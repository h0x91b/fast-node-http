{
	"defines": [
	],
	"targets": [
		{
			"target_name": "haywire",
			"product_name": "haywire",
			"type": "static_library",
			"msvs_guid": "5ECEC9E5-8F23-47B6-93E0-C3B328B3BE65",
			"dependencies": [
			],
			"conditions": [
				[
					"OS=='linux'",
					{
						"defines": [
							"PLATFORM_LINUX",
							"_GNU_SOURCE"
						],
						"cflags": [
							"-std=gnu99"
						]
					}
				],
				[
					"OS=='win'",
					{
						"defines": [
							"PLATFORM_WINDOWS"
						]
					},
					{
						"defines": [
							"PLATFORM_POSIX"
						]
					}
				]
			],
			"include_dirs": [
				"./include"
			],
			"sources": [
				"include/haywire.h",
				"src/haywire/configuration/configuration.h",
				"src/haywire/configuration/configuration.c",
				"src/haywire/configuration/ini.h",
				"src/haywire/configuration/ini.c",
				"src/haywire/connection_consumer.h",
				"src/haywire/connection_consumer.c",
				"src/haywire/connection_dispatcher.h",
				"src/haywire/connection_dispatcher.c",
				"src/haywire/http_connection.h",
				"src/haywire/http_parser.h",
				"src/haywire/http_parser.c",
				"src/haywire/http_request.h",
				"src/haywire/http_request.c",
				"src/haywire/http_response.h",
				"src/haywire/http_response.c",
				"src/haywire/http_response_cache.h",
				"src/haywire/http_response_cache.c",
				"src/haywire/http_server.h",
				"src/haywire/http_server.c",
				"src/haywire/hw_string.h",
				"src/haywire/hw_string.c",
				"src/haywire/khash.h",
				"src/haywire/route_compare_method.h",
				"src/haywire/route_compare_method.c",
				"src/haywire/server_stats.h",
				"src/haywire/server_stats.c"
			]
		},
		{
			"target_name": "haywire_shared",
			"product_name": "haywire",
			"type": "shared_library",
			"msvs_guid": "5ECEC9E5-8F23-47B6-93E0-C3B328B3BE65",
			"dependencies": [
			],
			"conditions": [
				[
					"OS=='linux'",
					{
						"defines": [
							"PLATFORM_LINUX",
							"_GNU_SOURCE"
						],
						"cflags": [
							"-std=gnu99"
						]
					}
				],
				[
					"OS=='win'",
					{
						"defines": [
							"PLATFORM_WINDOWS",
							"BUILDING_HAYWIRE_SHARED"
						]
					},
					{
						"defines": [
							"PLATFORM_POSIX"
						]
					}
				]
			],
			"include_dirs": [
				"./include"
			],
			"sources": [
				"include/haywire.h",
				"src/haywire/configuration/configuration.h",
				"src/haywire/configuration/configuration.c",
				"src/haywire/configuration/ini.h",
				"src/haywire/configuration/ini.c",
				"src/haywire/connection_consumer.h",
				"src/haywire/connection_consumer.c",
				"src/haywire/connection_dispatcher.h",
				"src/haywire/connection_dispatcher.c",
				"src/haywire/http_connection.h",
				"src/haywire/http_parser.h",
				"src/haywire/http_parser.c",
				"src/haywire/http_request.h",
				"src/haywire/http_request.c",
				"src/haywire/http_response.h",
				"src/haywire/http_response.c",
				"src/haywire/http_response_cache.h",
				"src/haywire/http_response_cache.c",
				"src/haywire/http_server.h",
				"src/haywire/http_server.c",
				"src/haywire/hw_string.h",
				"src/haywire/hw_string.c",
				"src/haywire/khash.h",
				"src/haywire/route_compare_method.h",
				"src/haywire/route_compare_method.c",
				"src/haywire/server_stats.h",
				"src/haywire/server_stats.c"
			]
		},
		{
			"include_dirs": [
				"<!(node -e \"require('nan')\")",
				"./include"
			],
			"dependencies": [
				"haywire"
			],
			"sources": [
				"fast-node-http.cc"
			],
			"target_name": "fast-node-http"
		}
	]
}
