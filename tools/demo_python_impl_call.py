from kwiver.vital import config
from kwiver.vital import plugin_management


def test_config_formatter_load_example():
    vpm = plugin_management.plugin_manager_instance()

    vpm.load_all_plugins()
    vpm.load_all_plugins()  # we should be able to do this idempotently
    vpm.load_all_plugins()

    cb_new = config.empty_config()

    impl_names = vpm.impl_names("format_config_block")
    print("Impl names for format_config_block: ", impl_names)

    cb_new.set_value("a", "1")
    cb_new.set_value("b", "2")
    cb_new.set_value("b:c", "other")


def test_say_example():
    vpm = plugin_management.plugin_manager_instance()
    vpm.load_all_plugins()

    impl_names = vpm.impl_names("Say")
    print("Impl names for say: ", impl_names)

    impl_names = ["cpp", "PythonImpl"]

    cb = config.empty_config()
    sf = plugin_management.SayFactory()

    for impl_name in impl_names:
        impl = sf.create(impl_name, cb)
        print("The", impl_name, "implementation says:")
        print(impl.says())
        print()


def test_they_say_example():
    vpm = plugin_management.plugin_manager_instance()
    vpm.load_all_plugins()

    they_names = ["cpp_they", "PythonTheyImpl"]
    speaker_names = ["cpp", "PythonImpl"]

    sf = plugin_management.SayFactory()

    print("Testing composite implementations")
    for t_name in they_names:
        for s_name in speaker_names:
            cb = config.empty_config()
            cb.set_value("speaker", s_name)

            impl = sf.create(t_name, cb)
            print(impl.says())


if __name__ == "__main__":
    test_config_formatter_load_example()
    test_say_example()
    test_they_say_example()
