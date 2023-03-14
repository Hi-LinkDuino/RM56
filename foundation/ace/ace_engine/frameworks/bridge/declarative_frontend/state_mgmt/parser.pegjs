program
 = code:code+ { return code.join(""); }

code
 = s:skipConsole { return s; }
  / c:. { return c; }

// Add "error" and/or "warn" if needed
func
 = f:("log" / "info" / "debug" ) { return f; }

console
 = head:"console" dot:(_ "." _) func:func _ { return head + '.' + func }

skipConsole
 = console:console "(" c:(!")" c:logChar { return c; })* ")"_";"? { return `/* ${console}(${c.join("")}); */`}

logChar
 = "(" c:logChar* ")" { return `(${c.join("")})`; }
 / "/*" c:logChar* "*/" { return `**${c.join("")}**`; }
 / !")"!"*/" c:. { return c; }

_ "whitespace"
  = [ ' '\t\n\r]*