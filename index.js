function reply_click(clicked_id) {
	var backgorund = document.getElementById("backgorund");
	var style = window.getComputedStyle(backgorund);
	var animationName = style.animationName;
	backgorund.style.animationName = animationName === "rotate" ? "off" : "rotate";
}

window.onload = function() {
	resize();
}

function resize() {
	var width = window.innerWidth;
	var height = window.innerHeight;
	var screenHypotenuse = Math.sqrt((width * width) + (height * height));
	var cssWidth = screenHypotenuse / width;
	var cssHeight = screenHypotenuse / height;
	var style = document.getElementById("backgorund").style;
	style.width = (cssWidth * 100) + "%";
	style.height = (cssHeight * 100) + "%";
	style.left = -((cssWidth - 1) * 50) + "%";
	style.top = -((cssHeight - 1) * 50) + "%";
}
