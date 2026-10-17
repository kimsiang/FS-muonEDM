"""Inline chet_engine.js into the live demo page -> chet_live.html"""
from pathlib import Path
here = Path(__file__).parent
page = (here / "chet_live.template.html").read_text().replace("/*__ENGINE__*/", (here / "chet_engine.js").read_text())
(here / "chet_live.html").write_text(page)
print("wrote", here / "chet_live.html", len(page), "bytes")
