#pragma once

namespace utility
{
	template<typename... Ts>
	struct overloads : Ts...
	{
		using Ts::operator()...;
	};
}
