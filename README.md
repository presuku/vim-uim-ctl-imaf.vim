# vim-uim-ctl-imaf.vim

This is uim (input method) control script for Vim with imactivefunc/imstatusfunc.

This vim script and c source files based on [uim-ctl](https://code.google.com/p/vim-soko/source/browse/trunk/uim-ctl) and [uim-ctlso](https://github.com/koron/imcsc-vim).

## How to install

* Download
  ```sh
$ cd ~/.vim/bundle
$ git clone https://github.com/presuku/vim-uim-ctl-imaf.vim.git
```

* Customize for IM Engine of your environment.

  You can change the followings tow macro in `plugin/uim-ctl.c`.

 * Default action-name for mozc

   ```c
#define ACTION_HIRAGANA "action_mozc_hiragana"
#define ACTION_DIRECT   "action_mozc_direct"
```

* Build
  ```sh
$ cd ~/.vim/bundle/vim-uim-ctl-imaf.vim
$ make
```

* Rename dll name

  If you want to rename uim-ctl.so for other environment, you can set shared object name to g:uim_ctl_dll.

 * rename
   ```sh
$ mv plugin/uim-ctl.so plugin/uim-ctl.so.1.7
# or
$ mv plugin/uim-ctl.so plugin/uim-ctl.so.1.8
```
  \# you can confirm your uim version by "uim-fep -v" or "uim-sh --version".

 * set shared object name to g:uim_ctl_dll in .vimrc
   ```viml
let s:uim_version = matchstr(system('uim-sh --version'), '\d.\d')
let g:uim_ctl_dll = "uim-ctl.so." . s:uim_version
```

* Vundle and others.
  ```viml
Bundle 'presuku/vim-uim-ctl-imaf.vim'
" or
Plugin 'presuku/vim-uim-ctl-imaf.vim'
" or
NeoBundle 'presuku/vim-uim-ctl-imaf.vim'
```


## LICENSE

All scripts are distributed under one of the Vim, MIT or BSD licenses.

