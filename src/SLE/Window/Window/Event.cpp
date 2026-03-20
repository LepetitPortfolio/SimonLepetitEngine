#include "Event.h"

template<typename TEventSubType>
Event::Event(const TEventSubType& _EventSubType)
{
	static_assert(IsEventSubType<TEventSubType>, "TEventSubtype must be a subtype of Event");

	if (IsEventSubType<TEventSubType>)
	{
		m_Data = _EventSubType;
	}
}

template<typename TEventSubType>
bool Event::Is() const
{
	static_assert(IsEventSubType<TEventSubType>, "TEventSubtype must be a subtype of Event");
	if (IsEventSubType<TEventSubType>)
	{
		return std::holds_alternative<TEventSubType>(m_Data);
	}
	return false;
}

template<typename TEventSubType>
const TEventSubType& Event::GetIf() const
{
	static_assert(IsEventSubType<TEventSubType>, "TEventSubtype must be a subtype of Event");
	if (IsEventSubType<TEventSubType>)
	{
		return std::get_if<TEventSubType>(m_Data);
	}

	return false;
}

template <typename Visitor>
decltype(auto) Event::Visit(Visitor&& _Visitor) const
{
	return std::visit(std::forward<Visitor>(_Visitor), m_Data);
}