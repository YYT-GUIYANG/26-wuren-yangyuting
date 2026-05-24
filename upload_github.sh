#!/bin/bash
#在作业2目录下运行: bash upload_github.sh
set -e
cd "$(dirname "$0")"
echo "当前目录: $(pwd)"
echo "文件列表:"
ls -la
if [ ! -d .git ]; then
    git init -b main
fi
git add README.md .gitignore \
    linear_regression.cpp ridge_regression.cpp prepare_data.py \
    01_多元线性回归.ipynb 02_正则化回归.ipynb \
    data/housing.csv data/train.csv data/test.csv 2>/dev/null || true
git add data/pred_linear.csv data/pred_ridge.csv 2>/dev/null || true
git status
git diff --cached --quiet && echo "没有新改动" || git commit -m "$(cat <<'EOF'
作业2：多元线性回归与Ridge回归

C++手搓回归类，加州房价数据，含notebook和预处理脚本。
EOF
)"
if ! gh auth status 2>/dev/null; then
    echo "请先登录GitHub: gh auth login"
    exit 1
fi
if ! git remote get-url origin 2>/dev/null; then
    REPO_NAME="homework2-linear-regression"
    gh repo create "$REPO_NAME" --public --source=. --remote=origin \
        --description "Homework2 linear and ridge regression" \
        || gh repo create "$REPO_NAME" --private --source=. --remote=origin \
        --description "Homework2 linear and ridge regression"
fi
git push -u origin "$(git branch --show-current)"
echo "完成! 仓库地址:"
gh repo view --json url -q .url 2>/dev/null || git remote get-url origin
