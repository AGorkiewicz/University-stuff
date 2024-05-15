#lang racket

(struct leaf () #:transparent)

(struct node2 (tag v1 v2) #:transparent)

(struct node3 (tag v1 v2 v3) #:transparent)

(define (sciezki u) ; Nie wiem, czy o to chodziło, treść nie była napisana zbyt jasno (wypisuję ścieżkę dla *każdego* liścia)
  (define (aux u now)
    (cond [(leaf? u)
           (list now)]
          [(node2? u)
           (let* ([now (append now (list (node2-tag u)))])
                  (append (aux (node2-v1 u) now)
                          (aux (node2-v2 u) now)))]
          [(node3? u)
           (let* ([now (append now (list (node3-tag u)))])
                  (append (aux (node3-v1 u) now)
                          (aux (node3-v2 u) now)
                          (aux (node3-v3 u) now)))]
          [else error "???"]))
  (aux u null))

(define A (node2 'A (leaf) (leaf)))
(define B (node3 'B (leaf) (leaf) (leaf)))
(define C (node2 'C A B))
(define D (node2 'D C (leaf)))

(sciezki D)



