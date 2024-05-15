#lang racket

(define (full-tree xs)
  (define (make-tree dep xs)
    (if (= dep 0)
        (cons (node (car xs) leaf leaf) (cdr xs))
        (let* ([left-res (make-tree (- dep 1) xs)]
               [left-tree (car left-res)]
               [left-list (cdr left-res)])
          (if (null? left-list)
              (cons left-tree null)
              (let* ([right-res (make-tree (- dep 1) (cdr left-list))]
                     [right-tree (car right-res)]
                     [right-list (cdr right-res)]
                     [my-tree (node (car left-list) left-tree right-tree)])
                (cons my-tree right-list))))))
  (car (make-tree (length xs) xs)))