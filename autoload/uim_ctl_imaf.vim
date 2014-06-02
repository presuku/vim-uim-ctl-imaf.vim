" vim:set sts=2 sw=2 tw=0 et:

" uim-ctl-imaf.vim based on uimfep-vim.vim, uim-ctlso.vim and uim-ctl.vim.

let s:save_cpo = &cpo
set cpo&vim

scriptencoding utf-8

augroup UimHelper
  au!
  autocmd VimLeave * call libcallnr(g:uim_ctl_imaf#dll_path, "unload", 0)
  " poll() does not work when dll is loaded before VimEnter.
  autocmd VimEnter * let s:err = libcallnr(g:uim_ctl_imaf#dll_path, 'load', g:uim_ctl_imaf#dll_path)
  autocmd VimEnter * if s:err != 0 | au! UimHelper * | endif
augroup END

function! g:uim_ctl_imaf#im_status_set(active)
  call libcallnr(g:uim_ctl_imaf#dll_path, 'im_set', a:active)
endfunction

function! g:uim_ctl_imaf#im_status_get()
  return libcallnr(g:uim_ctl_imaf#dll_path, 'is_im_enable', 0)
endfunction

let &cpo = s:save_cpo
unlet s:save_cpo

