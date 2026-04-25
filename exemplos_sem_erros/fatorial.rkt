(module main "main.rkt"
  (#%plain-module-begin
    (define-values (fact)
      (#%plain-lambda (n)
        (if (#%plain-app <= n (quote 1))
            (quote 1)
            (#%plain-app * n
              (#%plain-app fact
                (#%plain-app - n (quote 1)))))))
    (#%plain-app fact (quote 5))))
