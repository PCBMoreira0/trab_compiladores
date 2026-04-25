(module main "main.rkt"
  (#%plain-module-begin
    (define-values (f)
      (#%plain-lambda (x)
        (#%plain-app + x (quote 1))))))
