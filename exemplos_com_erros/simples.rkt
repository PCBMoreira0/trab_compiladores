(module main "main.rkt"
  (#%plain-module-begin
    (define-values x) (quot 10))
    (#%plain-app + x (quote 5))))