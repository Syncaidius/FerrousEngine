#pragma once
namespace fe {
	/* Generates all of the operators required to use an enum class as flags via bitwise operators. */
#define GEN_ENUM_CLASS_OPERATORS(TypeName) \
	inline TypeName operator | (TypeName l, TypeName r) { \
	using T = std::underlying_type_t <TypeName>; \
	return static_cast<TypeName>(static_cast<T>(l) | static_cast<T>(r));\
	} \
\
	inline TypeName& operator |= (TypeName & l, TypeName r)  { \
		l = l | r; \
		return l; \
	} \
\
	inline TypeName operator & (TypeName l, TypeName r) { \
		using T = std::underlying_type_t <TypeName>; \
		return static_cast<TypeName>(static_cast<T>(l) & static_cast<T>(r)); \
	} \
\
	inline TypeName& operator &= (TypeName & l, TypeName r){\
		l = l & r; \
		return l; \
	} \
\
	inline bool operator == (TypeName l, bool r) { \
		using T = std::underlying_type_t <TypeName>; \
		return static_cast<T>(l) > 0; \
	}
}