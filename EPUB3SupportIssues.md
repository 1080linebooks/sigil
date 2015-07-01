# Introduction #

EPUB 3 support in Sigil is currently lacking. Right now the approach is to add support for commonly requested EPUB 3 features that are currently being incorporated into EPUB 2 readers. Adding support for EPUB 3 is a non-trivial task and the right approach needs to be undertaken.

There are significant changes between EPUB 2 and EPUB 3. Sigil needs to support both formats to an extent. EPUB 2 cannot simply be dropped. However, care needs to be taken to prevent Sigil from becoming two applications (EPUB 2 and EPUB 3) more or less.

# Issues #

These issues apply to Sigil supporting EPUB 3. This is not an exhaustive list of every possible issue only known major issues. Also, this list does not include every change between EPUB 3 and EPUB 2. It only lists changes that make supporting EPUB 3 (while still supporting EPUB 2) difficult.

## Spec ##

### OPF ###

#### Metadata ####

The format of the meta element used to custom metadata has been redefined in EPUB 3. EPUB 3 mandates that the EPUB 2 (while not in error) meta element be ignored. However, the EPUB 3 format is invalid in EPUB 2.

#### Guide ####

Deprecated in favor of the EPUB Navigation Document landmarks feature.

### TOC ###

The NCX toc form EPUB 2 has been deprecated and a new format has been introduced for EPUB 3. The EPUB 3 TOC is not compatible with EPUB 2.

### Filenames ###

EPUB 2 spec states that the href is a URI. URI's have a restricted character set and allow for other characters to be represented though % encoding. EPUB 3 changes the href to allow IRI's which allow direct representation of the characters. An EPUB 2 with Non-encoded filenames will fail to work with a number of reading systems. At the same time a number of EPUB 3 reading system will fail to work with encoded filenames.

### Content ###

HTML 5 vs XHTML.
CSS 3 vs CSS 2.1.

### Scripting ###

EPUB 3 adds support for Javascript while this was explicitly forbidden in EPUB 2.

## General Operation ##

Saving as EPUB 3 vs EPUB 2.
Should opening an EPUB 2 "convert" to EPUB 3.

## Other Issues ##

### Libraries ###

FlightCrew needs to be updated for EPUB 3.

### Industry Adoption ###

We don't know what parts of EPUB 3 are important and should have priority focus. Specifically many companies are implementing select parts of EPUB 3. Other than Audio and Video there is no consensus on what parts of EPUB 3 are important. It doesn't make sense to focus on say the bindings element if reading systems are not going to implement this feature.

We need to chose those features that make the most sense and will have the greatest impact. However, without knowing what reading systems will even support it's hard to know what is worth spending time supporting.

# Possibile Solutions #

## Utilize EPUB 3's EPUB 2 Fallback Features ##

EPUB 3 defines a number of fallback features to allow an EPUB 3 to be compatible with EPUB 2. Files created in this manner may not necessarily be valid EPUB 2 but they should work with the majority of EPUB 2 reading systems.

This approach would mean Sigil only produces EPUB 3 files. There would be some type of option where EPUB 2 compatibility/fallback files would be generated and added to the EPUB.

### Advantages ###

It's easier to support one standard and produce compatibility/fallback files than supporting two standards. This would be implemented similar to how inline TOCs are implemented.

### Disadvantages ###

Sigil won't be able to save a true, fully valid EPUB 2.

## Drop EPUB 2 and Only Support EPUB 3 ##

### Advantages ###

Implementing EPUB 3 can happen very quickly. Sigil can have full EPUB 3 support without having to worry about how a change will affect EPUB 2.

### Disadvantages ###

Makes Sigil less useful. The majority of books and reading systems right now are EPUB 2 only.

## Don't Support EPUB 3 ##

### Advantages ###

No changes need to be made.

### Disadvantages ###

EPUB 3 (to some degree) is where the industry appears to be heading. Even companies like Apple and Kobo are supporting some aspects (not all) of the EPUB 3 spec.

## Support EPUB 3 Features in EPUB 2 ##

### Advantages ###

Current approach we're taking.

### Disadvantages ###

Only some features from EPUB 3 can be incorporated in this manner. Core features like the OPF and TOC changes can't be handled in this manner. Also, this requires reading systems to support these non-EPUB 2 features.