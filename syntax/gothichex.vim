" Vim syntax file
" Language:	hex file
" Last Change:	2011/07/04 15:39:42.
" Author:	goth_wrist_cut
" URL:		http://d.hatena.jp/goth_wrist_cut/

" Address := [0-9a-fA-F]\+
" Data    := [0-9a-fA-F]\{2\}
" Ascii   := .*
" Line    := /* Address */ Data* # Ascii

if exists( "b:current_syntax" )
	finish
endif

syntax match gothicHexAddress				"/\* *[0-9a-fA-F]\+ *\*/"	contains=gothicHexStartAddr,gothicHexEndAddr
syntax match gothicHexStartAddr		contained	"/\*"
syntax match gothicHexEndAddr		contained	"\*/"
syntax match gothicHexAscii				"// .*"				contains=gothicHexStartAscii
syntax match gothicHexStartAscii	contained	"//"

highlight link gothicHexAddress		Constant
highlight link gothicHexStartAddr	Identifier
highlight link gothicHexEndAddr		Identifier
highlight link gothicHexAscii		Statement
highlight link gothicHexStartAscii	Identifier

let b:current_syntax = "gothicHex"

