
from rover.main import main


def test_main_reports_scaffold(capsys):
    main()

    output = capsys.readouterr().out

    assert "Autonomous Adaptive 4WD Rover" in output
    assert "Software scaffold initialized." in output
    assert "Hardware is not initialized." in output
