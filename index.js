function reply_click(clicked_id)
{
	var backgorund = document.getElementById("backgorund");
	var style = window.getComputedStyle(backgorund);
	var animationName = style.animationName;
    backgorund.style.animationName = animationName === "rotate" ? "off" : "rotate";
}