set(test_exclusions)

# Calling virtual methods. See #2.
list(APPEND test_exclusions
  "^CXX::demo_python_impl_call$"
  "^Python::demo_python_impl_call$"
  )

string(REPLACE ";" "|" test_exclusions "${test_exclusions}")
if (test_exclusions)
  set(test_exclusions "(${test_exclusions})")
endif ()
