#include "machine.h"

#include "preproc_private.h"

machine_t machine_x86_64;
// machine_t machine_x86;
// machine_t machine_arm64;

#define PASTE2(a, b) a ## b
#define PASTE(a, b) PASTE2(a, b)
#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)
#define MACHINE_STR STRINGIFY(CUR_MACHINE)
#define INIT_PATHS \
	PASTE(machine_, CUR_MACHINE).npaths = 1 + npaths;
#define INCR_NPATHS(_path) \
	++PASTE(machine_, CUR_MACHINE).npaths;
#define DO_PATHS \
	if (!(PASTE(machine_, CUR_MACHINE).include_path =                                                                \
	      malloc(PASTE(machine_, CUR_MACHINE).npaths * sizeof *PASTE(machine_, CUR_MACHINE).include_path))) {        \
		printf("Failed to add include path to " MACHINE_STR " platform\n");                                          \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _nopath));                                                            \
	}                                                                                                                \
	failure_id = 0;                                                                                                  \
	ADD_PATH("include-fixed")                                                                                        \
	for (; failure_id < npaths + 1; ++failure_id) {                                                                  \
		if (!(PASTE(machine_, CUR_MACHINE).include_path[failure_id] = strdup(extra_include_path[failure_id - 1]))) { \
			printf("Failed to add include path to " MACHINE_STR " platform\n");                                      \
			goto PASTE(failed_, PASTE(CUR_MACHINE, _paths));                                                         \
		}                                                                                                            \
	}
#define ADD_PATH(path) \
	if (!(PASTE(machine_, CUR_MACHINE).include_path[failure_id] = strdup(path))) { \
		printf("Failed to add include path to " MACHINE_STR " platform\n");        \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _paths));                           \
	}                                                                              \
	++failure_id;
#define EXTRA_MACROS \
	PASTE(machine_, CUR_MACHINE).npredefs = PASTE(CUR_MACHINE, _NPREDEFS);                                       \
	if (!(PASTE(machine_, CUR_MACHINE).predef_macros_name =                                                      \
	      malloc((PASTE(CUR_MACHINE, _NPREDEFS)) * sizeof *PASTE(machine_, CUR_MACHINE).predef_macros_name))) {  \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");                                  \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _paths));                                                         \
	}                                                                                                            \
	if (!(PASTE(machine_, CUR_MACHINE).predef_macros =                                                           \
	      malloc((PASTE(CUR_MACHINE, _NPREDEFS)) * sizeof *PASTE(machine_, CUR_MACHINE).predef_macros))) {       \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");                                  \
		free(machine_x86_64.predef_macros_name);                                                                 \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _paths));                                                         \
	}                                                                                                            \
	failure_id = 0;
#define ADD_NAME(mname) \
	if (!(PASTE(machine_, CUR_MACHINE).predef_macros_name[failure_id] = strdup(#mname))) { \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");            \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _macros));                                  \
	}
#define ADD_MACRO(ntoks) \
	if (!(PASTE(machine_, CUR_MACHINE).predef_macros[failure_id] =                   \
	      malloc(sizeof *PASTE(machine_, CUR_MACHINE).predef_macros[failure_id]))) { \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");      \
		free(machine_x86_64.predef_macros_name[failure_id]);                         \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _macros));                            \
	}                                                                                \
	*PASTE(machine_, CUR_MACHINE).predef_macros[failure_id] = (macro_t){             \
		.is_funlike = 0,                                                             \
		.has_varargs = 0,                                                            \
		.nargs = 0,                                                                  \
		.toks = vector_new_cap(mtoken, (ntoks)),                                     \
	};                                                                               \
	++failure_id;                                                                    \
	if (!PASTE(machine_, CUR_MACHINE).predef_macros[failure_id - 1]->toks) {         \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");      \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _macros));                            \
	}
#define ADD_SYM(s) \
	mtok = mtoken_new_token((preproc_token_t){.tokt = PPTOK_SYM, .tokv.sym = SYM_ ## s}); \
	if (!mtok) {                                                                          \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");           \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _macros));                                 \
	}                                                                                     \
	vector_push(mtoken, PASTE(machine_, CUR_MACHINE).predef_macros[failure_id - 1]->toks, mtok);
#define ADD_STR(typ, n) \
	s = string_new_cstr(#n);                                                          \
	if (!s) {                                                                         \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");       \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _macros));                             \
	}                                                                                 \
	mtok = mtoken_new_token((preproc_token_t){.tokt = PPTOK_ ## typ, .tokv.str = s}); \
	if (!mtok) {                                                                      \
		printf("Failed to add predefined macro to " MACHINE_STR " platform\n");       \
		string_del(s);                                                                \
		goto PASTE(failed_, PASTE(CUR_MACHINE, _macros));                             \
	}                                                                                 \
	vector_push(mtoken, PASTE(machine_, CUR_MACHINE).predef_macros[failure_id - 1]->toks, mtok);

int init_machines(size_t npaths, const char *const *extra_include_path) {
	size_t failure_id;
	string_t *s;
	mtoken_t *mtok;
	
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
#define CUR_MACHINE x86_64
	machine_x86_64.size_long = 8;
	machine_x86_64.align_valist = 8;
	machine_x86_64.size_valist = 24;
	INIT_PATHS
#define DO_PATH INCR_NPATHS
#include "machine.gen"
#undef DO_PATH
	DO_PATHS
#define DO_PATH ADD_PATH
#include "machine.gen"
#undef DO_PATH
#define x86_64_NPREDEFS 9
	EXTRA_MACROS
	ADD_NAME(__x86_64__)
	ADD_MACRO(1)
	ADD_STR(NUM, 1)
	ADD_NAME(__WCHAR_MAX__)
	ADD_MACRO(1)
	ADD_STR(NUM, 2147483647)
	ADD_NAME(__WCHAR_MIN__)
	ADD_MACRO(5)
	ADD_SYM(LPAREN)
	ADD_SYM(DASH)
	ADD_STR(IDENT, __WCHAR_MAX__)
	ADD_SYM(DASH)
	ADD_STR(NUM, 1)
	ADD_NAME(__CHAR_BIT__)
	ADD_MACRO(1)
	ADD_STR(NUM, 8)
	ADD_NAME(__SCHAR_MAX__)
	ADD_MACRO(1)
	ADD_STR(NUM, 127)
	ADD_NAME(__SHRT_MAX__)
	ADD_MACRO(1)
	ADD_STR(NUM, 32767)
	ADD_NAME(__INT_MAX__)
	ADD_MACRO(1)
	ADD_STR(NUM, 2147483647)
	ADD_NAME(__LONG_MAX__)
	ADD_MACRO(1)
	ADD_STR(NUM, 9223372036854775807L)
	ADD_NAME(__LONG_LONG_MAX__)
	ADD_MACRO(1)
	ADD_STR(NUM, 9223372036854775807LL)
#undef CUR_MACHINE
#pragma GCC diagnostic pop
	
	return 1;
	
failed_x86_64_macros:
	while (failure_id--) {
		macro_del(machine_x86_64.predef_macros[failure_id]);
		free(machine_x86_64.predef_macros[failure_id]);
		free(machine_x86_64.predef_macros_name[failure_id]);
	}
	free(machine_x86_64.predef_macros);
	free(machine_x86_64.predef_macros_name);
	failure_id = machine_x86_64.npaths;
failed_x86_64_paths:
	while (failure_id--) {
		free(machine_x86_64.include_path[failure_id]);
	}
	free(machine_x86_64.include_path);
failed_x86_64_nopath:
	return 0;
}

static void machine_del(machine_t *m) {
	for (size_t predef_id = m->npredefs; predef_id--;) {
		macro_del(m->predef_macros[predef_id]);
		free(m->predef_macros[predef_id]);
		free(m->predef_macros_name[predef_id]);
	}
	free(m->predef_macros);
	free(m->predef_macros_name);
	for (size_t path_no = m->npaths; path_no--;) {
		free(m->include_path[path_no]);
	}
	free(machine_x86_64.include_path);
}
void del_machines(void) {
	machine_del(&machine_x86_64);
}

int validate_type(machine_t *target, type_t *typ) {
	if (typ->is_validated) return 1;
	typ->is_validated = 1;
	if (typ->is_restrict) {
		if (typ->typ != TYPE_PTR) {
			printf("Error: only pointers to object types may be restrict-qualified\n");
			return 0;
		}
		if (typ->val.typ->typ == TYPE_FUNCTION) {
			printf("Error: only pointers to object types may be restrict-qualified\n");
			return 0;
		}
	}
	if (typ->is_atomic) {
		if ((typ->typ == TYPE_ARRAY) || (typ->typ == TYPE_FUNCTION)) {
			printf("Error: array types and function types may not be atomic-qualified\n");
			return 0;
		}
	}
	switch (typ->typ) {
	case TYPE_BUILTIN:
		switch (typ->val.builtin) {
		case BTT_VOID:        typ->szinfo.align = typ->szinfo.size = 0; break;
		case BTT_BOOL:        typ->szinfo.align = typ->szinfo.size = 1; break;
		case BTT_CHAR:        typ->szinfo.align = typ->szinfo.size = 1; break;
		case BTT_SCHAR:       typ->szinfo.align = typ->szinfo.size = 1; break;
		case BTT_UCHAR:       typ->szinfo.align = typ->szinfo.size = 1; break;
		case BTT_SHORT:       typ->szinfo.align = typ->szinfo.size = 2; break;
		case BTT_SSHORT:      typ->szinfo.align = typ->szinfo.size = 2; break;
		case BTT_USHORT:      typ->szinfo.align = typ->szinfo.size = 2; break;
		case BTT_INT:         typ->szinfo.align = typ->szinfo.size = 4; break;
		case BTT_SINT:        typ->szinfo.align = typ->szinfo.size = 4; break;
		case BTT_UINT:        typ->szinfo.align = typ->szinfo.size = 4; break;
		case BTT_LONG:        typ->szinfo.align = typ->szinfo.size = target->size_long; break;
		case BTT_SLONG:       typ->szinfo.align = typ->szinfo.size = target->size_long; break;
		case BTT_ULONG:       typ->szinfo.align = typ->szinfo.size = target->size_long; break;
		case BTT_LONGLONG:    typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_SLONGLONG:   typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_ULONGLONG:   typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_INT128:      typ->szinfo.align = typ->szinfo.size = 16; break;
		case BTT_SINT128:     typ->szinfo.align = typ->szinfo.size = 16; break;
		case BTT_UINT128:     typ->szinfo.align = typ->szinfo.size = 16; break;
		case BTT_S8:          typ->szinfo.align = typ->szinfo.size = 1; break;
		case BTT_U8:          typ->szinfo.align = typ->szinfo.size = 1; break;
		case BTT_S16:         typ->szinfo.align = typ->szinfo.size = 2; break;
		case BTT_U16:         typ->szinfo.align = typ->szinfo.size = 2; break;
		case BTT_S32:         typ->szinfo.align = typ->szinfo.size = 4; break;
		case BTT_U32:         typ->szinfo.align = typ->szinfo.size = 4; break;
		case BTT_S64:         typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_U64:         typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_FLOAT:       typ->szinfo.align = typ->szinfo.size = 4; break;
		case BTT_CFLOAT:      typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_IFLOAT:      typ->szinfo.align = typ->szinfo.size = 4; break;
		case BTT_DOUBLE:      typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_CDOUBLE:     typ->szinfo.align = typ->szinfo.size = 16; break;
		case BTT_IDOUBLE:     typ->szinfo.align = typ->szinfo.size = 8; break;
		case BTT_LONGDOUBLE:  typ->szinfo.align = typ->szinfo.size = 16; break;
		case BTT_CLONGDOUBLE: typ->szinfo.align = typ->szinfo.size = 32; break;
		case BTT_ILONGDOUBLE: typ->szinfo.align = typ->szinfo.size = 16; break;
		case BTT_VA_LIST:     typ->szinfo.align = target->align_valist; typ->szinfo.size = target->size_valist; break;
		default:
			printf("Unknown builtin %u, cannot fill size info\n", typ->val.builtin);
			return 0;
		}
		return 1;
	case TYPE_ARRAY:
		if (typ->val.array.typ->is_incomplete || (typ->val.array.typ->typ == TYPE_FUNCTION)) {
			printf("Error: array types must point to complete object types\n");
			return 0;
		}
		if ((typ->val.array.typ->typ == TYPE_STRUCT_UNION) && typ->val.array.typ->val.st->has_incomplete) {
			printf("Error: array types may not (inductively) point to structures which last element is incomplete\n");
			return 0;
		}
		if ((typ->is_atomic) || (typ->is_const) || (typ->is_restrict) || (typ->is_volatile)) {
			// qualifier-type-list in array declaration is only allowed in function argument declaration under certain circumstances
			printf("Error: array types may not be qualified\n");
			return 0;
		}
		if (!validate_type(target, typ->val.array.typ)) return 0;
		if (typ->val.array.array_sz == (size_t)-1) {
			typ->szinfo.size = 0;
			typ->szinfo.align = (typ->val.array.typ->szinfo.align < 16) ? 16 : typ->val.array.typ->szinfo.align;
		} else {
			typ->szinfo.size = typ->val.array.array_sz * typ->val.array.typ->szinfo.size;
			typ->szinfo.align =
				((typ->szinfo.size >= 16) && (typ->val.array.typ->szinfo.align < 16)) ?
				16 :
				typ->val.array.typ->szinfo.align;
		}
		return 1;
	case TYPE_PTR:
		typ->szinfo.size = target->size_long;
		typ->szinfo.align = target->size_long;
		return validate_type(target, typ->val.typ);
	case TYPE_FUNCTION:
		if ((typ->val.fun.ret->typ == TYPE_FUNCTION) || (typ->val.fun.ret->typ == TYPE_ARRAY)) {
			printf("Error: function types may not return function or array types\n");
			return 0;
		}
		if (typ->val.fun.nargs != (size_t)-1) {
			for (size_t i = 0; i < typ->val.fun.nargs; ++i) {
				// Adjust the argument if necessary
				if (typ->val.fun.args[i]->typ == TYPE_ARRAY) {
					// Adjustment to pointer
					typ->val.fun.args[i]->typ = TYPE_PTR;
					typ->val.fun.args[i]->val.typ = typ->val.fun.args[i]->val.array.typ;
				} else if (typ->val.fun.args[i]->typ == TYPE_FUNCTION) {
					// Adjustment to pointer
					type_t *t2 = type_new_ptr(typ->val.fun.args[i]);
					if (!t2) {
						printf("Error: failed to adjust type of argument from function to pointer\n");
						return 0;
					}
					typ->val.fun.args[i] = t2;
				}
				if (!validate_type(target, typ->val.fun.args[i])) return 0;
			}
		}
		typ->szinfo.size = 0;
		typ->szinfo.align = 0;
		return validate_type(target, typ->val.fun.ret);
	case TYPE_STRUCT_UNION: {
		if (!typ->val.st->is_defined) return typ->is_incomplete;
		size_t max_align = 1, cur_sz = 0, cur_bit = 0;
		for (size_t i = 0; i < typ->val.st->nmembers; ++i) {
			// Adjust the argument if necessary
			st_member_t *mem = &typ->val.st->members[i];
			if (mem->typ->typ == TYPE_FUNCTION) {
				printf("Error: structures may not contain function members\n");
				return 0;
			}
			if (mem->typ->is_incomplete) {
				if ((i != typ->val.st->nmembers - 1) || !typ->val.st->is_struct || (mem->typ->typ != TYPE_ARRAY)) {
					// The last element of a structure may be a VLA
					printf("Error: structures may not contain incomplete members\n");
					return 0;
				}
				typ->val.st->has_incomplete = 1;
			}
			if (!validate_type(target, mem->typ)) return 0;
			if (!typ->val.st->is_struct && (mem->typ->typ == TYPE_STRUCT_UNION)) {
				typ->val.st->has_incomplete |= mem->typ->val.st->has_incomplete;
			}
			if (mem->is_bitfield) {
				if (!typ->val.st->is_struct) {
					printf("Error: TODO: bitfield in union\n");
					return 0;
				}
				if (mem->typ->is_atomic) {
					printf("Error: atomic bitfields are not supported\n");
					return 0;
				}
				if (mem->typ->typ != TYPE_BUILTIN) {
					printf("Error: bitfields can only have a specific subset of types\n");
					return 0;
				}
				if ((mem->typ->val.builtin != BTT_BOOL) && (mem->typ->val.builtin != BTT_INT)
				 && (mem->typ->val.builtin != BTT_SINT) && (mem->typ->val.builtin != BTT_UINT)) {
					printf("Error: bitfields can only have a specific subset of types\n");
					return 0;
				}
				if (!mem->name && (mem->typ->szinfo.align > max_align)) {
					printf("Error: TODO: unnamed bitfield member with greater alignment (width=%zu)\n", mem->bitfield_width);
					return 0;
				}
				if (mem->bitfield_width) {
					if (mem->name && (max_align < mem->typ->szinfo.align)) max_align = mem->typ->szinfo.align;
					size_t cur_block = cur_sz / mem->typ->szinfo.align;
					size_t end_block = (cur_sz + (cur_bit + mem->bitfield_width - 1) / 8) / mem->typ->szinfo.align;
					if (cur_block == end_block) {
						cur_bit += mem->bitfield_width;
						cur_sz += cur_bit / 8;
						cur_bit %= 8;
					} else {
						cur_sz = ((cur_sz + mem->typ->szinfo.align - 1) & ~(mem->typ->szinfo.align - 1)) + (mem->bitfield_width / 8);
						cur_bit = mem->bitfield_width % 8;
					}
				} else {
					if (max_align < mem->typ->szinfo.align) max_align = mem->typ->szinfo.align;
					cur_sz = ((cur_sz + mem->typ->szinfo.align - 1) & ~(mem->typ->szinfo.align - 1)) + mem->typ->szinfo.size;
					printf("Error: TODO: unnamed zero-width bitfield member\n");
					return 0;
				}
			} else {
				if (max_align < mem->typ->szinfo.align) max_align = mem->typ->szinfo.align;
				if (typ->val.st->is_struct) {
					if (cur_bit) {
						cur_bit = 0;
						++cur_sz;
					}
					cur_sz = ((cur_sz + mem->typ->szinfo.align - 1) & ~(mem->typ->szinfo.align - 1)) + mem->typ->szinfo.size;
				} else {
					if (cur_sz < mem->typ->szinfo.size) cur_sz = mem->typ->szinfo.size;
				}
			}
		}
		if (cur_bit) {
			cur_bit = 0;
			++cur_sz;
		}
		typ->szinfo.align = max_align;
		typ->szinfo.size = (cur_sz + max_align - 1) & ~(max_align - 1);
		return 1; }
	case TYPE_ENUM:
		if (typ->val.typ->typ != TYPE_BUILTIN) return 0;
		typ->szinfo = typ->val.typ->szinfo;
		return 1;
	}
	return 0;
}
