/*
 * Copyright 2013 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Original source is copyright 2010 - 2011. Alexey Tsoy.
 * http://sourceforge.net/projects/interpreter11/
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifdef BOOST_PP_IS_ITERATING

namespace detail {

template<typename R BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, typename ARG)>
struct lfn_invoker<R(BOOST_PP_ENUM_PARAMS(N,ARG))> {
	typedef R result_type;
	static const size_t argument_count = N;
	
	template<typename Function, typename Args>
	R operator()(Function & function, Args & args) const {
		
		#define BOOST_COMMAND_LINE_LFN_INVOKER(unused_1, i, unused_2) \
			BOOST_PP_COMMA_IF(i) args.template get<i>()
		
		return function(BOOST_PP_REPEAT_FROM_TO(0, N, BOOST_COMMAND_LINE_LFN_INVOKER, ~));
		
		#undef BOOST_COMMAND_LINE_LFN_INVOKER
	}
};

template<BOOST_PP_ENUM_PARAMS(N, typename ARG)>
struct lfn_invoker<void(BOOST_PP_ENUM_PARAMS(N,ARG))> {
	typedef void result_type;
	static const size_t argument_count = N;
	
	template<typename Function, typename Args>
	void operator()(Function & function, Args & args) const {
		
		#define BOOST_COMMAND_LINE_LFN_INVOKER(unused_1, i, unused_2) \
			BOOST_PP_COMMA_IF(i) args.template get<i>()
		
		function(BOOST_PP_REPEAT_FROM_TO(0, N, BOOST_COMMAND_LINE_LFN_INVOKER, ~));
		
		#undef BOOST_COMMAND_LINE_LFN_INVOKER
	}
};

} // namespace detail

#endif // BOOST_PP_IS_ITERATING
