The following steps should be performed before releasing a new carl-storm version.

1. Update `CHANGELOG.md`:
   * To get all the commits from an author since the last tag execute:
   ```console
   git log last_tag..HEAD --author "author_name"
   ```
   * Set release month

2. Check that carl builds without errors and all tests are successful:
   * [Github Actions](https://github.com/moves-rwth/carl-storm/actions/) should run successfully.

3. Set new carl version:
   * Set new version in `CMakeLists.txt`

4. Set new tag in Git (assuming that the new version is X.Y.Z and that the remote "origin" is the github repo).
   Use the flag `-s` to sign the tag.
   ```console
   git tag -a X.Y.Z -m "Carl version X.Y.Z" -s
   git push origin X.Y.Z
   ```
   The new tag should now be visible on [GitHub](https://github.com/moves-rwth/carl-storm/tags).

5. Use the [CI](https://github.com/moves-rwth/carl-storm/actions/workflows/release_docker.yml) on the tag, provide the version `X.Y.Z` as tag and automatically create the [Docker containers](https://hub.docker.com/r/movesrwth/carl-storm) for the new version.

6. [Add new release](https://github.com/moves-rwth/carl-storm/releases/new) in GitHub.

7. Update `stable` branch:

   ```console
   git checkout stable
   git rebase master
   git push origin stable
   ```
   Note: Rebasing might fail if `stable` is ahead of `master` (e.g. because of merge commits). In this case we can do:
    ```console
   git checkout stable
   git reset --hard master
   git push --force origin stable
   ```

8. Use the [CI](https://github.com/moves-rwth/carl-storm/actions/workflows/release_docker.yml) on the `stable` branch, provide the tag 'stable' and automatically create the [Docker containers](https://hub.docker.com/r/movesrwth/carl-storm).

9. Use the [CI of docker-storm](https://github.com/moves-rwth/docker-storm/actions/workflows/dependencies.yml) to update the Docker images for [storm-dependencies](https://hub.docker.com/r/movesrwth/storm-dependencies).
