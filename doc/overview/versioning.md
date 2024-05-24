# Release Lifecycle {#versioning_main}

[TOC]

# Overview {#versioning_overview}

https://semver.org/

* Major number - The major number indicates the current version of the package's public interface. 
  This should be incremented every time you make a change that would require existing users of your package to update their own work.
* Minor number - The minor number describes the current functional release of your software. 
  This is incremented whenever you add a new feature but do not otherwise alter your package's interface. 
  It communicates to users that a significant change has been made but the package remains fully backward compatible with the previous minor number.
* Patch number - The patch number gets incremented every time you make a minor change that neither impacts the public interface 
  or overall functionality of your package. This is most commonly used for bugfixes. 
  Consumers should always be able to update to the latest patch release without hesitation.
* Prerelease - prereleases are specified by a hyphen followed by one or more dot-separated prerelease identifiers. 
  Identifiers can include hyphen and alphanumeric characters. In prerelease, software may not be stable and might not satisfy the compatibility requirements 
  implied by the normal version.
* Build-identifiers - Build identifiers are specified specified by a plus sign followed by one or more dot-separated build identifiers. 
  Identifiers can include hyphen and alphanumeric characters. Build-identifiers should be used to mark project specific builds.
  
Both prerelease and build-identifiers are optional. Prerelease part comes before the build part.
  
# Release Timeline {#versioning_timeline}


* Major - As changes in the interface are hard to predict, there is no fixed timeline.
  But as a rule of the thump, a major change should not happen faster than a year after the last.
* Minor - Functional additions are collected and the decission of opening the development is beein done at the beginning of every PI (12 weeks)
  The minor change will happen at the end of the PI, so minor changes will not happen faster then 12 weeks, but might be longer.
* Patch number - They can change anytime, with the exception, that for every sprint (2 weeks) there is a jump to the next 100.

# Branching strategy  {#versioning_branching}

* All patch releasese are done on the main branch.
* For each major or minor release (x.y.0) a release branch is created.
* All breaking changes have to be done as release candidates on major release branches.
* After the release of breaking changes they have to be merged on the main branch.
* Only showstopper fixes will ever be merged on closed release branches.

@startuml

participant "main" as main
participant "release/1.0.0" as release10
participant "release/1.1.0" as release11
participant "release/2.0.0" as release20

activate main
hnote over main : v0.13.100
main ->(20) release10 : new major at end of PI
activate release10
hnote over release10 : v1.0.0-rc1
hnote over main : v0.13.200
main ->(20) release10 : merge
hnote over release10 : v1.0.0-rc2
hnote over release10 #FFAAAA: v1.0.0
release10 ->(20) main : merge breaking change
hnote over main : v1.0.100
main ->(20) release11 : new minor at end of PI
activate release11
hnote over main : v1.0.200
main ->(20) release11 : merge
hnote over release11 : v1.1.0-rc1
hnote over main : v1.0.300
main ->(20) release11 : merge
hnote over release11 : v1.1.0-rc2
hnote over release11 #FFFFAA: v1.1.0
hnote over main : v1.1.100
main ->(20) release20 : new major at end of PI
destroy release11
destroy release10
activate release20

@enduml


# Release details {#versioning_details}

## Major Release

### Lifetime

A major release is under development for at least one PI (12 weeks) and every major base release (xxx.0.zzz) will be supported for at least one year.
For each major release, the release branch is kept and critical fixes will be included for at least the last two releases.

### Change Control

As a major release is the only way, to do breaking changes to the API, the decission of creating one will not be taken lightly.
The CCB is at least the PO, architect and the project lead. With a new major release in development, all functions marked as depreciated are checked, 
if they will be dropped. All epics / features that will introduce a breaking change should be collected under the next major release and marked as on hold,
until the release is under development.

### Testing

A major release has to be tested in at least one project setup. While a breaking change is allowed, there should be a documentation of how to adapt to these changes.

### Compatibility
A major release is allowed to be incompatible to the previous version. It might require changes in the customer project to switch between major version.
To support customers the required changes should be documented and functions that will be dropped, should be marked depreciated in minor releases before the major release.
(API incompatible) 

### Release authority

Releases have to be cleared by PO, eco PO, test authority and project lead.

## Minor Release

### Lifetime

A minor release is under development for at least one PI (12 weeks) and every minor base release (xxx.yyy.0) will be supported for at least one PI.
For each minor release, the release branch is kept and critical fixes will be included for at least the last releases.

### Change Control

The CCB is at least the PO, architect. With a new minor release in development, new functions could be marked as depreciated but none will be dropped.
All epics / features that need additions to the API should be collected under the next minor release and marked as on hold,
until the release is under development.

### Testing

A minor release should be tested in project setup. No breaking changes are allowed.

### Compatibility
The executables of a given minor release should work together. 
Any library created from a minor release should be compilable to any customer project without changes in the customer code.
(API compatibility)

### Release authority

Releases have to be cleared by PO, eco PO and test authority.

## Patch Release

### Lifetime

A patch release can happen whenever needed. Patch releases happen from main branch and are not kept in a release branch after the release.

### Change Control

For patch releases, there is no fixed CCB. All files that are part of the API should be unchanged for a patch release, if possible. 
Additions are possible but not guaranteed to be stable until the next minor release. If in preparation for the next
minor release there are changes required, they will be under the CCB of that release and not promoted (documented / communicated)
until the next minor release

### Testing

A patch release should never break or change any valid integration tests. Patch releases should not change any interface functionality of the base minor release.

### Compatibility
A dynamic library created from a patch for a given minor release should be loadable by all versions of that minor release without recompiling.
(ABI compatibility)

### Release authority

Releases have to be cleared by PO and the test authority.

