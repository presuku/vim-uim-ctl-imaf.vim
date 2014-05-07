# vim-uim-ctl-imaf

This is uim (input method) control script for Vim with imactivefunc/imstatusfunc.

This vim script and c source files based on [uim-ctl](https://code.google.com/p/vim-soko/source/browse/trunk/uim-ctl) and [uim-ctlso](https://github.com/koron/imcsc-vim).

## Install

* Vundle and others.

  ```
Bundle 'presuku/vim-uim-ctl-imaf'
```

* Customize for IM Engine of your environment.
  You can change the followings tow macro in `plugin/uim-ctl.c`.

  Default action-name for mozc:
  ```
#define ACTION_HIRAGANA "action_mozc_hiragana"
#define ACTION_DIRECT   "action_mozc_direct"
```

* build uim-ctl.so and rename.

  ```
$ make
# you can confirm your uim version by "uim-fep -v".
$ mv plugin/uim-ctl.so plugin/uim-ctl.so.1.7
or
$ mv plugin/uim-ctl.so plugin/uim-ctl.so.1.8
```

## LICENSE

All scripts are distributed under one of the Vim, MIT or BSD licenses.

