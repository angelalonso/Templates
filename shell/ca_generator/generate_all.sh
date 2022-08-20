#!/usr/bin/env bash

OPENSSL_CMD="/usr/bin/openssl"
CRTPATH="./certs"
CFGSPATH="./base_cfgs"
COMMON_NAME="$1"

CA_CRT="$CRTPATH/cacert.pem"
CA_EXTFILE="$CRTPATH/cacert.cfg"
CA_KEY="$CRTPATH/ca.key"
EXTFILE="$CRTPATH/cert_ext.cfg"
SERVER_CONF="$CRTPATH/server_cert.cfg"
SERVER_CRT="$CRTPATH/server.crt"
SERVER_CSR="$CRTPATH/server.csr"
SERVER_EXT="$CRTPATH/server_ext.cfg"
SERVER_KEY="$CRTPATH/server.key"

function generate_selfsigned_cert {

  # generating server key
  echo "Generating private key"
  $OPENSSL_CMD genrsa -out $SERVER_KEY 4096 2>/dev/null
  if [ $? -ne 0 ] ; then
     echo "ERROR: Failed to generate $SERVER_KEY"
     exit 1
  fi

  ## Update Common Name in External File
  /bin/echo "commonName              = $COMMON_NAME" >> $EXTFILE

  # Generating Certificate Signing Request using config file
  echo "Generating Certificate Signing Request"
  $OPENSSL_CMD req -new -key $SERVER_KEY -out $SERVER_CSR -config $EXTFILE 2>/dev/null
  if [ $? -ne 0 ] ; then
     echo "ERROR: Failed to generate $SERVER_CSR"
     exit 1
  fi


  echo "Generating self signed certificate"
  $OPENSSL_CMD x509 -req -days 3650 -in $SERVER_CSR -signkey $SERVER_KEY -out $SERVER_CRT 2>/dev/null
  if [ $? -ne 0 ] ; then
     echo "ERROR: Failed to generate self-signed certificate file $SERVER_CRT"
  fi
}

function generate_root_ca {

    ## generate rootCA private key
    echo "Generating RootCA private key"
    if [[ ! -f $CA_KEY ]];then
       $OPENSSL_CMD genrsa -out $CA_KEY 4096 2>/dev/null
       [[ $? -ne 0 ]] && echo "ERROR: Failed to generate $CA_KEY" && exit 1
    else
       echo "$CA_KEY seems to be already generated, skipping the generation of RootCA certificate"
       return 0
    fi

    ## generate rootCA certificate
    echo "Generating RootCA certificate"
    #$OPENSSL_CMD req -new -x509 -days 3650 -config $CA_EXTFILE -key $CA_KEY -out $CA_CRT 2>/dev/null
    $OPENSSL_CMD req -new -x509 -days 3650 -config $CA_EXTFILE -key $CA_KEY -out $CA_CRT 
    [[ $? -ne 0 ]] && echo "ERROR: Failed to generate $CA_CRT" && exit 1

    ## read the certificate
    echo "Verify RootCA certificate"
    $OPENSSL_CMD  x509 -noout -text -in $CA_CRT >/dev/null 2>&1
    [[ $? -ne 0 ]] && echo "ERROR: Failed to read $CA_CRT" && exit 1
}

function generate_server_certificate {

    echo "Generating server private key"
    $OPENSSL_CMD genrsa -out $SERVER_KEY 4096 2>/dev/null
    [[ $? -ne 0 ]] && echo "ERROR: Failed to generate $SERVER_KEY" && exit 1

    echo "Generating certificate signing request for server"
    $OPENSSL_CMD req -new -key $SERVER_KEY -out $SERVER_CSR -config $SERVER_CONF 2>/dev/null
    [[ $? -ne 0 ]] && echo "ERROR: Failed to generate $SERVER_CSR" && exit 1

    echo "Generating RootCA signed server certificate"
    $OPENSSL_CMD x509 -req -in $SERVER_CSR -CA $CA_CRT -CAkey $CA_KEY -out $SERVER_CRT -CAcreateserial -days 365 -sha512 -extfile $SERVER_EXT 2>/dev/null
    [[ $? -ne 0 ]] && echo "ERROR: Failed to generate $SERVER_CRT" && exit 1

    echo "Verifying the server certificate against RootCA"
    $OPENSSL_CMD verify -CAfile $CA_CRT $SERVER_CRT >/dev/null 2>&1
     [[ $? -ne 0 ]] && echo "ERROR: Failed to verify $SERVER_CRT against $CA_CRT" && exit 1
}

function show_usage {

    printf "Usage: $0 [options [parameters]]\n"
    printf "\n"
    printf "Options:\n"
    printf " -cn, Provide Common Name for the certificate\n"
    printf " -h|--help, print help section\n"

    return 0
}


# MAIN
case $1 in
     -cn)
         shift
         COMMON_NAME="$1"
         ;;
     --help|-h)
         show_usage
         exit 0
         ;;
     *)
        ## Use hostname as Common Name
        COMMON_NAME=`/usr/bin/hostname`
        ;;
esac

# generate CRTPATH if it doesn't exist
#   and copy over the configs we will use
mkdir -p $CRTPATH
cp $CFGSPATH/* $CRTPATH

generate_selfsigned_cert
generate_root_ca
generate_server_certificate
