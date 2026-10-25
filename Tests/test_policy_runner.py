"""Exercise the Linux policy runner without the Unreal SDK."""
from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]


class PolicyRunner(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="passenger-runner-")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.project = self.base / "project with spaces"
        for name in (
            "Tests/run-policy-tests.sh",
            "Tests/StandalonePassengerPolicyTests.cpp",
            "Source/TsabsPassengerWagon/Public/PassengerAccessPolicy.h",
        ):
            source = ROOT / name
            self.assertTrue(source.is_file(), f"Missing runner input: {name}")
            target = self.project / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)
        self.scratch = self.base / "temporary files"
        self.scratch.mkdir()

    def run_runner(self, compiler=None):
        env = os.environ.copy()
        env["TMPDIR"] = str(self.scratch)
        if compiler is not None:
            env["CXX"] = compiler
        return subprocess.run(
            ["bash", str(self.project / "Tests/run-policy-tests.sh")],
            cwd=self.base,
            env=env,
            capture_output=True,
            text=True,
            timeout=60,
        )

    def test_real_suite_runs_both_modes_from_an_unrelated_directory(self):
        result = self.run_runner()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(result.stdout.count("26 checks passed"), 2)
        self.assertFalse(any(self.scratch.iterdir()))
        self.assertFalse((self.project / ".test-bin").exists())

    def test_compiler_failure_stops_before_a_second_attempt(self):
        compiler = self.base / "failing compiler"
        marker = self.base / "compiler calls"
        compiler.write_text(
            '#!/bin/sh\nprintf "called\\n" >> "$COMPILER_CALLS"\nexit 37\n'
        )
        compiler.chmod(0o755)
        with patch.dict(os.environ, {"COMPILER_CALLS": str(marker)}):
            result = self.run_runner(str(compiler))
        self.assertEqual(result.returncode, 37)
        self.assertEqual(marker.read_text().splitlines(), ["called"])
        self.assertNotIn("checks passed", result.stdout)
        self.assertFalse(any(self.scratch.iterdir()))

    def test_test_failure_is_propagated_and_temporary_files_are_removed(self):
        (self.project / "Tests/StandalonePassengerPolicyTests.cpp").write_text(
            "int main() { return 23; }\n"
        )
        result = self.run_runner()
        self.assertEqual(result.returncode, 23, result.stdout + result.stderr)
        self.assertNotIn("checks passed", result.stdout)
        self.assertFalse(any(self.scratch.iterdir()))


if __name__ == "__main__":
    unittest.main(verbosity=2)
