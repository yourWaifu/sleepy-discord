function reply_click(clicked_id) {
	var backgorund = document.getElementById("backgorund");
	var style = window.getComputedStyle(backgorund);
	var transformMatrix = style.transform;
	backgorund.style.animationDuration = style.animationDuration === "0s" ? style.transitionDuration : "0s";
	backgorund.style.transform = transformMatrix;
}

window.onload = function() {
	resize();
	var backgorund = document.getElementById("backgorund");
	backgorund.style.transitionDuration = window.getComputedStyle(backgorund).animationDuration;	//this is where we store the animationDuration for later
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
	var mainSectionRect = document.getElementById("mainSection").getBoundingClientRect();
	if (mainSectionRect.bottom < height) {
		var mainSectionStyle = document.getElementById("mainSection").style;
		mainSectionStyle.height = (height - mainSectionRect.top) + "px"
	}
}
