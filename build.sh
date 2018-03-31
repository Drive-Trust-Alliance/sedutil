dev_branch="sedutils"

git checkout $dev_branch
git fetch && git rebase --autostash FETCH_HEAD
git checkout integration
git pull
git merge $dev_branch
git push
git checkout $dev_branch
