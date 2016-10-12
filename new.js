//load youtube iframe player api
var tag = document.createElement("script");
tag.src = "https://www.youtube.com/iframe_api";
var firstScriptTag = document.getElementsByTagName('script')[0];
firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);

var width = window.innerWidth;
var height = window.innerHeight;
var aspectRatio_x = 16;
var aspectRatio_y = 9;
var video = { "videoId": "gNbSjMFd7j4", "startSeconds": 23, "endSeconds": 129 };
var defaults = { autoplay: 1, autohide: 0, modestbranding: 0, rel: 0, showinfo: 0, controls: 0, disablekb: 1, enablejsapi: 0, iv_load_policy: 3 };

//create <iframe>
var player;
function onYouTubeIframeAPIReady() {
	player = new YT.Player('player', {
		height: height,
		width: width,
		videoId: video.videoId,
		startSeconds: 23,
		endSeconds: 129,
		playerVars: defaults,
		events: {
			'onReady': onPlayerReady,
			'onStateChange': onPlayerStateChange
		}
	});
}

function onPlayerReady(){
	player.mute();
	player.loadVideoById(video);
	resizeVideo();
}

function onPlayerStateChange(event) {
	switch (player.getPlayerState()) {
		case 0: player.seekTo(video.startSeconds, true); break;
		case 2: player.playVideo(); break;
	}
}

function resize() {
	resizeVideo();
}

function resizeVideo() {
	width = window.innerWidth;
	height = window.innerHeight;
	var realWidth;
	var e = document.getElementById("video");
	if (width / height > aspectRatio_x / aspectRatio_y) {
		player.setSize(width, width / aspectRatio_x * aspectRatio_y);
		e.style.left = 0 + "px";
	} else {
		realWidth = height / aspectRatio_y * aspectRatio_x;
		player.setSize(realWidth, height);
		e.style.left = -((realWidth - width) / 2 ) + "px";
	}
}
