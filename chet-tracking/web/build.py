"""Inline chet_engine.js into the live demo page -> chet_live.html"""
from pathlib import Path
here = Path(__file__).parent
import base64
page = (here / "chet_live.template.html").read_text().replace("/*__ENGINE__*/", (here / "chet_engine.js").read_text())
page = page.replace("/*__BLENDER_JPG__*/", base64.b64encode((here / "chet_event_blender.jpg").read_bytes()).decode())
(here / "chet_live.html").write_text(page)
print("wrote", here / "chet_live.html", len(page), "bytes")

# ---- standalone copy for GitHub Pages (or any static host): a full HTML document
head, body = page.split('<div class="app"', 1)
site = f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<meta name="description" content="Live muEDM CHeT event display: simulated decay positrons tracked and helix-fitted in the browser.">
<style>[hidden]{{display:none!important}} body{{margin:0}} img{{max-width:100%}}
:root{{padding-top:env(safe-area-inset-top,0px);padding-bottom:env(safe-area-inset-bottom,0px)}}</style>
{head.strip()}
</head>
<body>
<div class="app"{body}
</body>
</html>
"""
docs = here.parent / "docs"
docs.mkdir(exist_ok=True)
(docs / "index.html").write_text(site)
(docs / ".nojekyll").write_text("")
print("wrote", docs / "index.html", len(site), "bytes")
