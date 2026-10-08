# Rights and license policy — P0

## Repository role

This repository is a **fork with Ninja history**. Fork ownership does not transfer
authorship or copyright in inherited Ninja material.

## Upstream Ninja boundary

Historical/inherited Ninja material retains its original authorship, copyright
notices, and upstream license terms.

The file `COPYING` at repository root is restored verbatim from the pre-rewrite
Ninja-derived base commit:

`4e4df1e567eb3c1475a51af261cba2bfff60b4be`

That upstream license must not be rewritten, replaced, or presented as a license
grant by Rafael Melo Reis.

`UPSTREAM_LICENSE != RAFCODE_LICENSE`

## RAFCODE authored boundary

New RAFCODE-authored material identifies:

**Rafael Melo Reis — 2026**

No outbound software license for that authored material is selected or granted by
this correction.

`LICENSE_STATE=TOKEN_VAZIO`
`OUTBOUND_LICENSE_GRANT=false`

Until the copyright owner explicitly selects exact license text/version:

- do not add an SPDX license identifier to RAFCODE-authored files;
- do not infer Apache-2.0 from the fact that this repository is a Ninja fork;
- do not infer a license from public visibility, GitHub hosting, or forkability;
- do not redistribute RAFCODE-authored material under a license not explicitly authorized.

## Intake gate

For every third-party component record:

`SOURCE -> AUTHOR/RIGHTSHOLDER -> LICENSE -> OBLIGATIONS -> COMPATIBILITY -> ATTRIBUTION -> EVIDENCE`

Unknown or ambiguous rights state -> `TOKEN_VAZIO -> BLOCKED`.

`PUBLIC != PUBLIC_DOMAIN`  
`ATTRIBUTION != PERMISSION`  
`REPOSITORY_OWNER != AUTHOR`  
`FORK != COPYRIGHT_TRANSFER`  
`UPSTREAM_LICENSE != AUTOMATIC_LICENSE_FOR_NEW_CODE`
