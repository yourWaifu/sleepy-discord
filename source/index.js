/* This is no longer needed. Delete if you want. */

function reply_click(clicked_id) {
	var backgorund = document.getElementById("backgorund");
	var style = window.getComputedStyle(backgorund);
	backgorund.style.animationPlayState = style.animationPlayState === "running" ? "paused" : "running";
}

window.onload = function() {
	resize();
}

function resize() {
	var width = window.innerWidth;
	var height = window.innerHeight;
	//resize backgorund
	var screenHypotenuse = Math.sqrt((width * width) + (height * height));
	var cssWidth = screenHypotenuse / width;
	var cssHeight = screenHypotenuse / height;
	var backgorundStyle = document.getElementById("backgorund").style;
	backgorundStyle.width = (cssWidth * 100) + "%";
	backgorundStyle.height = (cssHeight * 100) + "%";
	backgorundStyle.left = -((cssWidth - 1) * 50) + "%";
	backgorundStyle.top = -((cssHeight - 1) * 50) + "%";
	//resize main content
	var mainDivRect = document.getElementById("mainDiv").getBoundingClientRect();
	var mainSectionStyle = document.getElementById("mainSection").style;
	if (mainDivRect.bottom < height) {
		var mainSectionRect = document.getElementById("mainSection").getBoundingClientRect();
		mainSectionStyle.height = (height - mainSectionRect.top) + "px"
	} else {
		mainSectionStyle.height = "100%";
	}
}
