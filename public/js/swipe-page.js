$( document ).on( "pageinit", "[data-role='page'].tank-controls", function() {
	var page = "#" + $( this ).attr( "id" ),
		next = $( this ).jqmData( "next" ),
		prev = $( this ).jqmData( "prev" );
	
	if ( next ) {
		$.mobile.loadPage( next + ".html" );
		$( document ).on( "swipeleft", page, function() {
			$.mobile.changePage( next + ".html", { transition: "slide" });
		});
		$( ".control .next", page ).on( "click", function() {
			$.mobile.changePage( next + ".html", { transition: "slide" } );
		});
	}
	else {
		$( ".control .next", page ).addClass( "ui-disabled" );
	}
	if ( prev ) {
		$( document ).on( "swiperight", page, function() {
			$.mobile.changePage( prev + ".html", { transition: "slide", reverse: true } );
		});
		$( ".control .prev", page ).on( "click", function() {
			$.mobile.changePage( prev + ".html", { transition: "slide", reverse: true } );
		});
	}
	else {
		$( ".control .prev", page ).addClass( "ui-disabled" );
	}
});

