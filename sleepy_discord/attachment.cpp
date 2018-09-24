#include "attachment.h"

namespace SleepyDiscord {
	//Attachment::Attachment() :
	//	size(0), height(0), width(0)
	//{}

	//Attachment::~Attachment() {
	////}
	//
	//Attachment::Attachment(const std::string * rawJSON) : Attachment(json::getValues(rawJSON->c_str(), fields)) {}

	//Attachment::Attachment(const nonstd::string_view& rawJSON) : Attachment(json::getValues(rawJSON.data(), fields)) {}

	//Attachment::Attachment(const json::Values values) :
	//	//variable           condition  modifier    value                     felid
	//	Parent    (                                 values[index(fields, "id"       )]                ),
	//	filename  (                                 values[index(fields, "filename" )]                ),
	//	size      ( std::stoull(                    values[index(fields, "size"     )].to_string()   )),
	//	url       (                                 values[index(fields, "url"      )]                ),
	//	proxy_url (                                 values[index(fields, "proxy_url")]                ),
	//	height    (modIfElse(isDefined, std::stoul, values[index(fields, "height"   )].to_string(), 0)),
	//	width     (modIfElse(isDefined, std::stoul, values[index(fields, "width"    )].to_string(), 0))
	//{}
	//const std::initializer_list<const char*const> Attachment::fields = {
	//	"id", "filename", "size", "url", "proxy_url", "height", "width"
	//};

	Attachment::Attachment(const json::Value& json) :
		Attachment(json::fromJSON<Attachment>(json)) {
	}

	Attachment::Attachment(const nonstd::string_view& json) :
		Attachment(json::fromJSON<Attachment>(json)) {
	}
}