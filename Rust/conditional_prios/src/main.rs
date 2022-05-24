use rustlogic;
use std::collections::HashMap;


#[derive(Clone, Debug)]
struct LogicDict {
    formula: String,
    varset: HashMap<String, bool>,
}

// TODO:
// || and && changed to | and &

fn do_comps(s: &str, vars: HashMap<String, i32>) -> bool {
    let mut prev_char: String = String::new();
    let mut var1: String = String::new();
    let mut oper: String = String::new();
    let mut var2: String = String::new();
    for c in s.chars() {
        match c {
            // 
            '=' | '>' | '<' => {
                if prev_char != "var1" && prev_char != "oper" { panic!("Wrong syntax at {}", s) }
                oper.push(c);
                prev_char = "oper".to_string()
            }
            // anything else should be part of a variable name
            _ => {
                match prev_char.as_str() {
                    "var2" => { var2.push(c); }
                    _ => { 
                        if prev_char == "oper" {
                            prev_char = "var2".to_string();
                            var2.push(c); 
                        } else {
                            prev_char = "var1".to_string();
                            var1.push(c); 
                        }
                    }
                }
            }
        }
    }
    match oper.as_str() {
        "=" | "==" => { vars.get(&var1) == vars.get(&var2) }
        ">" => { vars.get(&var1) > vars.get(&var2) }
        "<" => { vars.get(&var1) < vars.get(&var2) }
        ">=" => { vars.get(&var1) >= vars.get(&var2) }
        "<=" => { vars.get(&var1) <= vars.get(&var2) }
        "!=" => { vars.get(&var1) != vars.get(&var2) }
        _  => { false }
    }
}

fn get_comps(s: &str, vars: HashMap<String, i32>) -> String {
    let mut result = String::new();
    let mut r = s.clone().to_string();
    // need this to trigger one last run of do_comps
    // TODO: find a better way
    r.push('#');
    let mut expr: String = String::new();
    let mut prev_char: String = String::new();
    let cleaned_r = r
        .replace("&&", "&")
        .replace("||", "|");
    for c in cleaned_r.chars() {
        match c {
            // possible delimiter
            '(' | ')' | ' ' | '&' | '|' | '!' | '#' => {
                if prev_char == "var2" {
                    for ch in do_comps(&expr, vars.clone()).to_string().chars() {
                        result.push(ch);
                    }
                    expr.clear();
                }
                if c != '#' { result.push(c); }
                prev_char = "limit".to_string();
            }
            // anything else should be part of a variable name
            '=' | '>' | '<' => {
                expr.push(c);
                prev_char = "comp".to_string();
            }
            // anything else should be part of a variable name
            _ => {
                expr.push(c);
                match prev_char.as_str() {
                    "comp" | "var2" => { prev_char = "var2".to_string(); }
                    _ => { prev_char = "var".to_string(); }
                }
            }

        }
    }
    result
}

fn parse_to_rustlogic(s: &str) -> LogicDict {
    let mut vars: HashMap<String, bool> = HashMap::new();
    let mut tbd = true;
    let mut var_ix = 0;
    //let mut formula: String = String::new();
    let mut formula = s.to_string();
    while tbd == true {
        let form_aux: String;
        if formula.contains("false") {
            var_ix +=1;
            form_aux = formula.clone().replacen("false", &format!("[var{}]", var_ix), 1);
            vars.insert(format!("var{}", var_ix), false);
            formula = form_aux;
        } else if formula.contains("true") {
            var_ix +=1;
            form_aux = formula.clone().replacen("true", &format!("[var{}]", var_ix), 1);
            vars.insert(format!("var{}", var_ix), true);
            formula = form_aux;
        } else {
            tbd = false;
        }
    }
    let result = LogicDict {
        formula: formula.to_string(),
        varset: vars,
    };

    result.clone()

}

fn get_result(v: &str, vars: HashMap<String, i32>) -> bool {
    let comps = get_comps(v, vars);
    let logicvars = parse_to_rustlogic(&comps); 

    let formula =
        rustlogic::parse(&logicvars.formula).expect("OH OHHHH...");
    
    let mut variable_map = HashMap::new();

    let mut aux: &str;
    for k in logicvars.varset.keys() {
        aux = &k;
        variable_map.insert(aux, logicvars.varset[k]);
    }

    formula.get_value_from_variables(&variable_map).unwrap()
}

fn main() {
    let mut vars = HashMap::new();
    vars.insert("a".to_string(), 5);
    vars.insert("b".to_string(), 5);
    vars.insert("c".to_string(), 5);
    vars.insert("d".to_string(), 5);
    vars.insert("e".to_string(), 5);
    vars.insert("f".to_string(), 5);
    vars.insert("g".to_string(), 5);
    vars.insert("h".to_string(), 5);
    let v = "(a<=b||c>=d)&&e>f";
    println!("{:?}", get_result(v, vars));

}

