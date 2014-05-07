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

let s:uim_version = system('uim-fep -v')
if s:uim_version =~# 'uim-fep version 1.7.\d*.'
  let s:dll = expand("<sfile>:p:h") . "/uim-ctl.so.1.7"
elseif s:uim_version =~# 'uim-fep version 1.8.\d*.'
  let s:dll = expand("<sfile>:p:h") . "/uim-ctl.so.1.8"
else
  let s:dll = expand("<sfile>:p:h") . "/uim-ctl.so"
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

