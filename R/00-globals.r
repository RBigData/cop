# -----------------------------------------------------------------------------
# MPI globals
# -----------------------------------------------------------------------------

REDUCE_TO_ALL = -1L



# -----------------------------------------------------------------------------
# type globals
# -----------------------------------------------------------------------------

TYPE_DOUBLE = 1L
TYPE_FLOAT = 2L

TYPES_STR = c("double", "float")
TYPES_INT = 1:2
names(TYPES_INT) = TYPES_STR

type_int2str = function(type)
{
  TYPES_STR[type]
}

type_str2int = function(type)
{
  TYPES_INT[[type]]
}
