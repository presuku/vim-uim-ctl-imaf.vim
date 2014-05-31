" vim:set sts=2 sw=2 tw=0 et:

" uim-ctl-imaf.vim based on uimfep-vim.vim, uim-ctlso.vim and uim-ctl.vim.

scriptencoding utf-8

" Check imactivatefunc (and imstatusfunc)
if !exists('&imactivatefunc')
  finish
endif

if exists("s:init")
  finish
endif
let s:init = 1

if exists("g:uim_ctl_dll")
  let s:dll = expand("<sfile>:p:h") . "/" . g:uim_ctl_dll
else
  let s:dll = expand("<sfile>:p:h") . "/" . "uim-ctl.so"
endif

augroup UimHelper
  au!
  autocmd VimLeave * call libcallnr(s:dll, "unload", 0)
  " poll() does not work when dll is loaded before VimEnter.
  autocmd VimEnter * let s:err = libcallnr(s:dll, 'load', s:dll)
  autocmd VimEnter * if s:err != 0 | au! UimHelper * | endif
augroup END

function! UimGet()
  return libcallnr(s:dll, 'is_im_enable', 0)
endfunction

function! UimSet(active)
  call libcallnr(s:dll, 'im_set', a:active)
endfunction

set imactivatefunc=UimSet
set imstatusfunc=UimGet

