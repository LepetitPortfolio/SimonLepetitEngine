#include "Event_cls.h"

template<typename TEventSubType>
Event_cls::Event_cls(const TEventSubType& _EventSubType)
{
	static_assert(IsEventSubType<TEventSubType>, "TEventSubtype must be a subtype of Event_cls");

	if (IsEventSubType<TEventSubType>)
	{
		m_Data = _EventSubType;
	}
}

template<typename TEventSubType>
bool Event_cls::Is() const
{
	static_assert(IsEventSubType<TEventSubType>, "TEventSubtype must be a subtype of Event_cls");
	if (IsEventSubType<TEventSubType>)
	{
		return std::holds_alternative<TEventSubType>(m_Data);
	}
	return false;
}

template<typename TEventSubType>
const TEventSubType& Event_cls::GetIf() const
{
	static_assert(IsEventSubType<TEventSubType>, "TEventSubtype must be a subtype of Event_cls");
	if (IsEventSubType<TEventSubType>)
	{
		return std::get_if<TEventSubType>(m_Data);
	}

	return false;
}

template <typename Visitor>
decltype(auto) Event_cls::Visit(Visitor&& _Visitor) const
{
	return std::visit(std::forward<Visitor>(_Visitor), m_Data);
}