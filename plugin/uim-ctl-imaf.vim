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

let g:uim_ctl_dll_path = expand("<sfile>:p:h") . "/" . get(g:, "uim_ctl_dll", "uim-ctl.so")

augroup UimHelperInit
  au!
  autocmd InsertEnter * let s:err = libcallnr(g:uim_ctl_dll_path, 'load', g:uim_ctl_dll_path) | autocmd! UimHelperInit
augroup END

augroup UimHelperFinalize
  au!
  autocmd VimLeave * call libcallnr(g:uim_ctl_dll_path, "unload", 0)
augroup END

command! -nargs=1 UimCtlSet :call g:uim_ctl_imaf#im_status_set(<q-args>)
command! -nargs=0 UimCtlGet :call g:uim_ctl_imaf#im_status_get_msg()

set imactivatefunc=g:uim_ctl_imaf#im_status_set
set imstatusfunc=g:uim_ctl_imaf#im_status_get


let &cpo = s:save_cpo
unlet s:save_cpo

