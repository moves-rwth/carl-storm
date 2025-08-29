The following steps should be performed before releasing a new carl-storm version.

1. Update `CHANGELOG.md`:
   * Set release month
   * Set major changes.
     Use the [automatically generated release notes](https://docs.github.com/en/repositories/releasing-projects-on-github/automatically-generated-release-notes).
     Alternatively, get all the commits since the `last_tag` by executing:
     ```console
     git log last_tag..HEAD
     ```

2. Check that carl-storm [CI](https://github.com/moves-rwth/carl-storm/actions/) builds without errors and all tests are successful.

3. Set new carl-storm version in `CMakeLists.txt`.

4. (The tag can also automatically be set in the next step when creating the release on Github.)
   Set the new tag in Git, use the flag `-s` to sign the tag.
   ```console
   git tag -a X.Y -m "Carl version X.Y" -s
   git push origin X.Y
   ```
   The new tag should now be visible on [GitHub](https://github.com/moves-rwth/carl-storm/tags).

5. [Add new release](https://github.com/moves-rwth/carl-storm/releases/new) on GitHub.
   Create a new tag or use the tag created in the previous step.
   Finishing the release automatically triggers a CI workflow which also
   * updates the `stable` branch
   * creates new Docker containers for both the tag and `stable` branch
   * triggers a PR in Storm to update the carl-storm version
