from rover import service

def test_service_has_safe_entrypoint():
    assert callable(service.main)
