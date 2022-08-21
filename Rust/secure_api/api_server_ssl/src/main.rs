#[macro_use]
extern crate actix_web;

use actix_web::{middleware, App, HttpServer};
use native_tls;
use openssl::ssl::{SslAcceptor, SslFiletype, SslMethod};
use std::io::Read;
use std::path::Path;
use std::process::Command;
use std::{env, io};

mod constants;
mod like;
mod response;
mod tweet;

const CERTPATH: &str = "./certs";

fn are_ca_n_cert_ok() -> bool {
    let all_files = [
        "ca.key",
        "cacert.pem",
        "server.key",
        "server.csr",
        "server.crt",
    ];
    for f in all_files {
        let filepath = format!("{}/{}", CERTPATH, f);
        let m = Path::new(&filepath).exists();
        if !m {
            return false;
        }
    }
    true
}

fn generate_ca_n_cert() -> io::Result<()> {
    let mut we_are_ca = Command::new("./generate_ca.sh");
    let exit_status = we_are_ca.status().expect("process failed to execute");
    if exit_status.code() == Some(0) {
        return Ok(());
    }
    Err(io::Error::new(
        io::ErrorKind::Other,
        "We could not become CA!",
    ))
}

pub fn tls_acceptor() -> native_tls::TlsAcceptor {
    let mut file = std::fs::File::open("cert+key.p12")
        .map_err(|e| {
            println!("failed to open .p12");
            println!("try running: openssl pkcs12 -export -passout pass:'' -out cert+key.p12 -inkey key.pem -in cert.pem");
            e
        }).expect("opening .p12");
    let mut der = vec![];
    file.read_to_end(&mut der).unwrap();
    let cert = native_tls::Identity::from_pkcs12(&der, "").expect("failed to read .p12");
    let tls_cx = native_tls::TlsAcceptor::builder(cert).build().unwrap();
    native_tls::TlsAcceptor::from(tls_cx)
}

#[actix_rt::main]
async fn main() -> io::Result<()> {
    if !are_ca_n_cert_ok() {
        eprintln!("INFO: generating our own CA and creating Certificate");
        generate_ca_n_cert().unwrap();
    }

    let mut builder = SslAcceptor::mozilla_intermediate(SslMethod::tls()).unwrap();
    builder
        .set_private_key_file("certs/server.key", SslFiletype::PEM)
        .unwrap();
    builder
        .set_certificate_chain_file("certs/server.crt")
        .unwrap();

    env::set_var("RUST_LOG", "actix_web=debug,actix_server=info");
    env_logger::init();

    HttpServer::new(|| {
        App::new()
            // enable logger - always register actix-web Logger middleware last
            .wrap(middleware::Logger::default())
            .service(tweet::list)
            .service(tweet::get)
            .service(tweet::create)
            .service(tweet::delete)
    })
    // TODO: set port up manually
    .bind_openssl("127.0.0.1:8080", builder)?
    .run()
    .await
    // TEST: curl --cacert certs/cacert.pem -X GET https://127.0.0.1:8080/tweets
}
