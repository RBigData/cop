# -----------------------------------------------------------------------------
# MPI globals
# -----------------------------------------------------------------------------

REDUCE_TO_ALL = -1L



# -----------------------------------------------------------------------------
# type globals
# -----------------------------------------------------------------------------

TYPES_STR_NUMERIC = c("double", "float")
TYPES_STR_INTEGER = c("int", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t")
TYPES_STR = c(TYPES_STR_NUMERIC, TYPES_STR_INTEGER)

TYPES_INT = 1:length(TYPES_STR)
names(TYPES_INT) = TYPES_STR

type_int2str = function(type)
{
  TYPES_STR[type]
}

type_str2int = function(type)
{
  TYPES_INT[[type]]
}
