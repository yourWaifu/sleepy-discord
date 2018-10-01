#include "embed.h"

namespace SleepyDiscord {
	EmbedThumbnail::EmbedThumbnail(const json::Value & json) :
		EmbedThumbnail(json::fromJSON<EmbedThumbnail>(json)) {
	}
	EmbedThumbnail::EmbedThumbnail(const nonstd::string_view & json) :
		EmbedThumbnail(json::fromJSON<EmbedThumbnail>(json)) {
	}

	EmbedVideo::EmbedVideo(const json::Value & json) :
		EmbedVideo(json::fromJSON<EmbedVideo>(json)) {
	}
	EmbedVideo::EmbedVideo(const nonstd::string_view & json) :
		EmbedVideo(json::fromJSON<EmbedVideo>(json)) {
	}

	EmbedImage::EmbedImage(const json::Value & json) :
		EmbedImage(json::fromJSON<EmbedImage>(json)) {
	}
	EmbedImage::EmbedImage(const nonstd::string_view & json) :
		EmbedImage(json::fromJSON<EmbedImage>(json)) {
	}

	EmbedProvider::EmbedProvider(const json::Value & json) :
		EmbedProvider(json::fromJSON<EmbedProvider>(json)) {
	}
	EmbedProvider::EmbedProvider(const nonstd::string_view & json) :
		EmbedProvider(json::fromJSON<EmbedProvider>(json)) {
	}

	EmbedAuthor::EmbedAuthor(const json::Value & json) :
		EmbedAuthor(json::fromJSON<EmbedAuthor>(json)) {
	}
	EmbedAuthor::EmbedAuthor(const nonstd::string_view & json) :
		EmbedAuthor(json::fromJSON<EmbedAuthor>(json)) {
	}

	EmbedFooter::EmbedFooter(const json::Value & json) :
		EmbedFooter(json::fromJSON<EmbedFooter>(json)) {
	}
	EmbedFooter::EmbedFooter(const nonstd::string_view & json) :
		EmbedFooter(json::fromJSON<EmbedFooter>(json)) {
	}

	EmbedField::EmbedField(const json::Value & json) :
		EmbedField(json::fromJSON<EmbedField>(json)) {
	}
	EmbedField::EmbedField(const nonstd::string_view & json) :
		EmbedField(json::fromJSON<EmbedField>(json)) {
	}

	Embed::Embed(const json::Value & json) :
		Embed(json::fromJSON<Embed>(json)) {
	}
	Embed::Embed(const nonstd::string_view & json) :
		Embed(json::fromJSON<Embed>(json)) {
	}
}