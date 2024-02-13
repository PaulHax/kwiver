set(test_exclusions)

# format:
# Add one per line:
# "^name_of_test$"
list(APPEND test_exclusions
  )

string(REPLACE ";" "|" test_exclusions "${test_exclusions}")
if (test_exclusions)
  set(test_exclusions "(${test_exclusions})")
endif ()
