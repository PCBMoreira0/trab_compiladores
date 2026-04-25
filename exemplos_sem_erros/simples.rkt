(module main "main.rkt"
  (#%plain-module-begin
    (define-values (x) (quote 10))
    (#%plain-app + x (quote 5))))
