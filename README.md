# EMFexplorer

<p align="center">  
  <img src="images/banner.jpg" alt="EMFexplorer - A GDI+ Experiment" width="800">
</p>

**EMFexplorer** is a legacy C++/MFC graphics utility designed to parse, inspect, and render Windows Enhanced Metafiles (`.emf`). Originally developed in 2004, this project served as a technical implementation for exploring GDI+ and low-level Windows graphics structures.

### The Smoothing Proof

When rendering complex technical drawings, the difference between the standard Windows GDI and EMFexplorer's custom GDI+ treatment is night and day.

| Original PDF | EMF | EMF |
| :---: | :---: | :---: |
| <img src="images/compare_pdf.jpg" width="220" alt="Original PDF" /> | <img src="images/compare_gdi.jpg" width="220" alt="Standard GDI" /> | <img src="images/compare_emfexplorer.jpg" width="220" alt="EMFexplorer Rendering" /> |
| **Original PDF**<br><sub>(by Adobe Acrobat)</sub> | **EMF**<br><sub>(by Windows GDI)</sub> | **EMF**<br><sub>(by EMFexplorer)</sub> |

<p align="center">
  <i>Figure: EMFexplorer smoothing and anti-aliasing comparison.</i>
</p>

> **Note:** The example was chosen (images are not falsified) to make the point as clear as possible; of course not all GDI outputs are that bad.

---

## ⚠️ Project Status: Archived
This repository is a clean, historical archive of the original 2004 codebase. 
* **Status:** Stable / Locked.
* **Maintenance:** No active development, updates, or bug fixes will be provided.
* **Purpose:** Preserved solely for educational, historical, and reverse-engineering reference.

## Project & Package Architecture
The underlying engine bridges high-level document handling with low-level vector manipulation. The graphics library specializes in parsing and reusing documents formatted as Enhanced Metafile Format (`.emf`), Windows Metafile (`.wmf`), and GDI+ supported bitmap formats (`.bmp`, `.jpeg`, `.png`, `.tiff`, `.gif`).

<p align="center">  
  <img src="images/architecture.jpg" alt="EMFexplorer Package Architecture" width="650">
</p>

### Key Subsystems (2004 Implementation)
* **Graphics Engine:** Custom parsing abstraction layer built to isolate and reconstruct scalable vector graphic layers.
* **Demonstration Subsystem (`EMFexplorer.exe`):** The main interactive desktop client containing the visual layout analyzer and step-by-step record inspector tree.
* **ActiveX Controller (`SCEMFAx.ocx`):** A scriptable component designed for historical web integration, supporting progressive image streaming, parsing, and real-time canvas rendering.

---

## Historical Test Suites & Assets
The development and quality control of EMFexplorer relied on multi-platform metafile evaluation. The classic validation packages have been bundled into a single downloadable testing package containing distinct verification suites:

📦 **[Download the Testing Corpus (testfiles.zip)](testfiles.zip)** *(~35 MB)*

### The `.bgp` (Bound Graphics Pages) Format
The `MiniCorpus` utilizes EMFexplorer's native **`.bgp`** format—a specialized document container structure designed to pack multiple standalone pages (vector `.emf` or standard raster files) into an ordered graphic collection. 
* **Browsing Collections:** To explore a collection, open a directory within the test suite using `EMFexplorer.exe` and execute/double-click the master `.bgp` indexing file.
* **Reflowing Capabilities:** The project features a built-in **Pages Manager** (accessible via the `Document -> [Reflow...]` menu route), allowing developers to order, swap, append clipboard structures, or re-index multi-page graphic albums interactively.

### Included Suites
* **MiniCorpus:** The curated mini Caplife corpus featuring multi-layered, highly complex vector structures arranged into `.bgp` volumes to push layout engines to their rendering limits. Includes historical deep-dives like *Discover Haiti*, *Discover France*, and regional profile logs.
<details>
<summary>🔍 Click to view the Caplife Collection Index (Full File Checklist)</summary>

*(The full list of `.bgp` file paths covering the root disclaimer, FICA Soccer, Discover France, and Discover Haiti suites is included in the referenced web document.)*

</details>
* **emf_tests:** A collection of small, targeted test files ideal for isolating individual GDI/GDI+ record types, tree parsing parameters, and regression boundaries.
* **ActiveXTests:** Specialized test environments featuring compressed Enhanced Metafile format profiles (`.emz`), preserved directly from the historical web integration and progressive streaming development framework.

> 💡 **Technical Note on `.emz` Files:** The `.emz` files found in the `ActiveXTests` suite are GZIP-compressed Enhanced Metafiles (`.emf`) originally optimized for progressive web streaming. While modern web browsers completely deprecate ActiveX container hosting, the vector data remains fully accessible. You can manually unpack them by renaming the extension to `.gz` and extracting them via standard tools like 7-Zip, or programmatically decompress them using `zlib` to obtain the raw underlying `.emf` structure.

---

## Features (2004 Context)
* Low-level parsing of GDI/GDI+ Metafile structures.
* Visual tree representation of EMF records and parameters.
* Direct rendering preview using Win32 / MFC graphics pipelines.
* Export capabilities to standard formats available at the time.

## Repository Cleanup
The codebase has been cleaned for modern archival:
* Removed all legacy IDE temporary build artifacts (`.ncb`, `.opt`, `.plg`, `Debug/`, `Release/`).
* Standardized under the permissive **MIT License**.
* Retained raw source code (`.cpp`, `.h`, `.rc`) and assets.

## Documentation & Continuity
For context regarding the architecture or to follow current engineering notes, visit the main laboratory:
👉 **[drcharleslabs.com](https://drcharleslabs.com)**

---
*Copyright © 2004-2026 Smith Charles. Released under the MIT License.*
