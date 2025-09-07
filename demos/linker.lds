SECTIONS {
	. = 0x0,
	.text: : { 
		* (.text_start);
		* (.text); 
	}
}
