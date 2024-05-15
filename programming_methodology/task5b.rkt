#lang racket

(require "sudoku-spec.rkt")

(provide
 rows->board board-rows board-columns board-boxes ; Zad A
 partial-solution-ok? solutions)                  ; Zad B


;; =========
;; Zadanie A  (wszystko liniowo) :O
;; =========


(define (trans xss) ; transpozycja listy 2D (liniowo)
  (define (kebab xss) ; para <odcięta kolumna, pozostałość po cięciu>
    (define (merge now res)
      (cons (cons (car now)
                  (car res))
            (cons (cdr now)
                  (cdr res))))
    (foldr merge (cons null null) xss))
  (if (null? (car xss))
      null
      (let ([res (kebab xss)])
        (cons (car res) (trans (cdr res))))))


(define (2d->1d xss) ; spłaszcza listę 2D->1D
  (foldr append null xss))


(define (1d->2d xs) ; lista 1D dł. len² -> lista 2D o wym. len x len
  (let ([len (root (length xs))])
    (define (merge now res)
      (let ([curlen (car res)] [parts (cdr res)])
        (if (= curlen len)
            (cons 1
                  (cons (list now) parts))
            (cons (+ (car res) 1)
                  (cons (cons now (car parts)) (cdr parts))))))
    (cdr (foldr merge (cons len null) xs))))


(define (map2d f xss) ; map dla list 2D
  (map (λ (xs) (map f xs)) xss))


(define (root n) ; sqrt(n)
  (define (root-iter now)
    (if (= (* now now) n)
        now
        (root-iter (+ now 1))))
  (root-iter 0))


(define (rows->board xss) ; robi listę 4D (liniowo)
  (map trans (1d->2d (map 1d->2d xss))))

(define (board-rows board) ; swap na wymiarach 2-3 -> merge wymiarów 1-2, 3-4
  (board-boxes (map trans board)))

(define (board-columns board) ; swap na wymiarach 3-4, 1-2, 2-3 -> merge wymiarów 1-2, 3-4
  (board-rows (trans (map2d trans board))))

(define (board-boxes board) ; merge wymiarów 1-2, 3-4
  (2d->1d (map2d 2d->1d board)))

; testy
#|
(define tab4x4 '((1 2 3 4)
                 (5 6 7 8)
                 (9 a b c)
                 (d e f g)))

(define tab9x9 '((a1 a2 a3 a4 a5 a6 a7 a8 a9)
                 (b1 b2 b3 b4 b5 b6 b7 b8 b9)
                 (c1 c2 c3 c4 c5 c6 c7 c8 c9)
                 (d1 d2 d3 d4 d5 d6 d7 d8 d9)
                 (e1 e2 e3 e4 e5 e6 e7 e8 e9)
                 (f1 f2 f3 f4 f5 f6 f7 f8 f9)
                 (g1 g2 g3 g4 g5 g6 g7 g8 g9)
                 (h1 h2 h3 h4 h5 h6 h7 h8 h9)
                 (i1 i2 i3 i4 i5 i6 i7 i8 i9)))

(define board1 (rows->board tab4x4))
(define board2 (rows->board tab9x9))

(board-rows board2)
(board-columns board2)
(board-boxes board2)
|#


;; =========
;; Zadanie B  O(szybko)
;; =========


(define (fill-front xs n val) ; dodaj na początek n razy val
  (if (= n 0)
      xs
      (fill-front (cons val xs) (- n 1) val)))


(define vac '_) ; puste pole


(define (combine-lists xs ys) ; zwraca "złożenie" spłaszczonych plansz lub #f jeśli są niezgodne
  (define (combine-elems x y) ; combine dla pary elementów; zwraca listę lub #f jeśli się nie udało
    (cond [(eq? x vac) (list y)]
          [(eq? y vac) (list x)]
          [(not (ch-eq? x y)) #f]
          [else (list x)]))
  (if (null? xs)
      null
      (let ([res (combine-lists (cdr xs) (cdr ys))]
            [now (combine-elems (car xs) (car ys))])
        (if (and res now)
            (cons (car now) res)
            #f))))

  
(define (board-check board) ; sprawdza poprawność wypełnienia boarda (czy nie ma powtórzeń)
  (define (lists-check xss) ; sprawdza, czy w listach na liście nie ma powtórzeń
    (define (list-check xs) ; sprawdza, czy w liście nie ma powtórzeń
      (let ([sorted (sort (filter (λ (x) (not (eq? x vac))) xs) ch-lt?)])
        (define (merge now res)
          (cond [(not res) #f]
                [(null? res) (list now)]
                [(ch-eq? (car res) now) #f]
                [else (list now)]))
        (list? (foldr merge null sorted))))
    (andmap list-check xss))
  (and (lists-check (board-rows board))
       (lists-check (board-columns board))
       (lists-check (board-boxes board))))

         
(define (partial-solution-ok? initial-board elems rows)
  (let* ([init (board-rows initial-board)]
         [size (length init)]
         [suff (append elems (2d->1d rows))]
         [full (fill-front suff (- (* size size) (length suff)) vac)]
         [flat (2d->1d init)]
         [comb (combine-lists full flat)])
    (if comb
        (board-check (rows->board (1d->2d comb)))
        #f)))

; przykłady

#|
(provide alphabet ch-eq? ch-lt?)
(define alphabet '(1 2 3 4 5 6 7 8 9))
(define ch-eq? =)
(define ch-lt? <)
(partial-solution-ok?
 (rows->board '((_ _ 4 _) (_ 3 _ _) (1 2 _ 4) (_ 4 _ _)))
 '(2 1)
 '((1 2 3 4) (3 4 1 2)))
|#

;; ----------------------

;; Lista poprawnych częściowych wierszy po dodaniu
;; nowego znaku do jakiegoś częściowego rozwiązania
;; (elems, rows)
(define (new-elems initial-board elems rows)
  (filter
   (λ(es) (partial-solution-ok? initial-board es rows))
   (map (λ(c) (cons c elems)) alphabet)))

;; Lista poprawnych wierszy możliwych do dodania do
;; częściowego rozwiązania zawierającego tylko pełne
;; wiersze (rows)
(define (new-rows initial-board rows)
  (map (λ(r) (cons r rows))
       (foldr (λ(_ ess)
                (append-map
                 (λ(es) (new-elems initial-board es rows))
                 ess))
              (list null)
              alphabet)))

;; Lista poprawnych pełnych rozwiązań
(define (solutions initial-board)
  (map (λ(s) (rows->board s))
       (foldr (λ(_ rss)
                (append-map
                 (λ(rs) (new-rows initial-board rs))
                 rss))
              (list null)
              alphabet)))