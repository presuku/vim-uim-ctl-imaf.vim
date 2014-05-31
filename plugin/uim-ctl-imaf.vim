" vim:set sts=2 sw=2 tw=0 et:

" uim-ctl-imaf.vim based on uimfep-vim.vim, uim-ctlso.vim and uim-ctl.vim.

let s:save_cpo = &cpo
set cpo&vim

scriptencoding utf-8

" Check imactivatefunc (and imstatusfunc)
if !exists('&imactivatefunc')
  finish
endif

if exists("g:loaded_vim_uim_ctl_imaf")
  finish
endif
let g:loaded_vim_uim_ctl_imaf = 1

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

let &cpo = s:save_cpo
unlet s:save_cpo

