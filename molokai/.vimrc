colorscheme molokai
 
set t_Co=256

syntax on
" set nu!
set background=dark
" set paste 可以在粘贴的时候不会有变型
"set paste

" 突出显示当行
set cursorline
set cul

" 突出显示当列
set cursorcolumn
set cuc

set showmatch
set fileencodings=utf-8,ucs-bom,gb18030,gbk,gb2312,cp936
set termencoding=utf-8
set encoding=utf-8
set tabstop=4
set shiftwidth=4
set autoindent
set laststatus=2
set ruler
filetype on

set listchars=tab:>-,trail:-

set hlsearch
set incsearch

set smarttab
set noexpandtab
set nocompatible

" 统一缩进为4
set softtabstop=4
set autowrite
set cindent

"set rtp+=~/.vim/bundle/Vundle.vim

"set runtimepath+=~/.vim/bundle/YouCompleteMe

set laststatus=2

" 为C程序提供自动缩进
set smartindent

