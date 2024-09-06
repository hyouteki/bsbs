;;; bsbsfile-mode.el --- Major mode for editing bsbsfile configuration files -*- lexical-binding: t; -*-

;; Author: hyouteki
;; URL: https://github.com/hyouteki/bsbs/blob/master/editor_plugins/bsbsfile-mode.el
;; Version: 0.1
;; Keywords: languages
;; Package-Requires: ((emacs "24.3"))

(defvar bsbsfile-mode-hook nil)

(defvar bsbsfile-mode-map
  (let ((map (make-keymap)))
    map)
  "Keymap for `bsbsfile-mode'.")

(setq bsbsfile-keywords '("let" "label" "section" "in" "run" "end"))
(setq bsbsfile-constants '("Verbose" "None" "Continue" "Exit"))
(setq bsbsfile-variables '("Log" "OnError"))

(setq bsbsfile-keywords-regexp (regexp-opt bsbsfile-keywords 'words))
(setq bsbsfile-constants-regexp (regexp-opt bsbsfile-constants 'words))
(setq bsbsfile-variables-regexp (regexp-opt bsbsfile-variables 'words))
(setq bsbsfile-variable-applications-regexp "\\$[a-zA-Z_][a-zA-Z0-9_]*")

(setq bsbsfile-font-lock-keywords
      `(
        (,bsbsfile-keywords-regexp . font-lock-keyword-face)
        (,bsbsfile-constants-regexp . font-lock-constant-face)
        (,bsbsfile-variables-regexp . font-lock-variable-name-face)
        (,bsbsfile-variable-applications-regexp . font-lock-variable-name-face)
        ))

(define-derived-mode bsbsfile-mode fundamental-mode "bsbsfile"
  "Major mode for editing bsbsfile configuration files."
  (setq font-lock-defaults '((bsbsfile-font-lock-keywords))))

(add-to-list 'auto-mode-alist '("\\bsbsfile\\'" . bsbsfile-mode))

(provide 'bsbsfile-mode)
